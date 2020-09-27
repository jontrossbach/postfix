#FROM registry.redhat.io/ubi8/ubi
#https://serverfault.com/questions/1034517/how-can-i-enable-powertools-repository-in-centos-rhel-8
#subscription error preventing me from using UBI8:
#This system has no repositories available through subscriptions.
FROM centos:8

RUN dnf -y install git vim net-tools
RUN dnf -y group install "Development Tools"
RUN dnf -y install postfix
RUN dnf -y install dnf-plugins-core
RUN dnf -y install https://dl.fedoraproject.org/pub/epel/epel-release-latest-8.noarch.rpm
RUN dnf config-manager --set-enabled PowerTools
RUN dnf -y install libdb libdb-devel gcc openssl openssl-devel pcre pcre-devel openldap-devel cyrus-sasl cyrus-sasl-devel openldap libnsl2-devel libnsl postgresql postgresql-devel
RUN dnf -y install make automake #gcc gcc-c++ kernel-devel

RUN git clone https://github.com/jontrossbach/postfix.git

RUN cd postfix/cf/ && mv main.cf /etc/postfix/main.cf && mv master.cf /etc/postfix/master.cf
RUN echo "postfix:*:12345:12345:postfix:/no/where:/no/shell" >> /etc/passwd && echo "postfix:*:12345:" >> /etc/group && echo "postdrop:*:54321:" >> /etc/group

RUN pwd
RUN ls

#RUN ln -s /usr/include/libdb4/db.h /usr/include/db.h

RUN cd /postfix/postfix #&& make
RUN cd /postfix/postfix && echo "make makefiles CCARGS='-DHAS_PGSQL -I/usr/local/include/pgsql -fPIC -DUSE_TLS -DUSE_SSL -DUSE_SASL_AUTH -DUSE_CYRUS_SASL -DPREFIX=\\"/usr\\" -DHAS_LDAP -DLDAP_DEPRECATED=1 -DHAS_PCRE -I/usr/include/openssl -I/usr/include/sasl  -I/usr/include' AUXLIBS='-L/usr/local/lib -lpq -L/usr/lib64 -L/usr/lib64/openssl -lssl -lcrypto -L/usr/lib64/sasl2 -lsasl2 -lpcre -lz -lm -lldap -llber -Wl,-rpath,/usr/lib64/openssl -pie -Wl,-z,relro' OPT='-O' DEBUG='-g'" > build-postfix.sh && chmod a+x build-postfix.sh
RUN cd /postfix/postfix && ./build-postfix.sh 
RUN cd /postfix/postfix && make 
RUN cd /postfix/postfix && make tidy 
RUN cd /postfix/postfix && ./build-postfix.sh 
RUN cd /postfix/postfix && make upgrade #&& dnf -exclude postfix
#RUN ifconfig eth0:1 127.0.0.1 netmask 255.255.255.255 up
RUN chmod -R 777 /var/spool/postfix/ && chmod -R 777 /var/lib/postfix


RUN echo excludepkgs=postfix >> /etc/dnf/dnf.conf

USER postfix

EXPOSE 2525

ENTRYPOINT postfix start-fg
