# -*- mode: ruby -*-
# vi: set ft=ruby :
#
# Copy this file to ``Vagrantfile`` and customize it as you see fit.

VAGRANTFILE_API_VERSION = "2"

Vagrant.configure(VAGRANTFILE_API_VERSION) do |config|
 # If you'd prefer to pull your boxes from Hashicorp's repository, you can
 # replace the config.vm.box and config.vm.box_url declarations with the line below.
 #
 # config.vm.box = "fedora/30-cloud-base"
 #config.vm.box = "peru/ubuntu-18.04-server-amd64"
 #config.vm.box_version = "20200806.01"
 #config.vm.box = "f32-cloud-libvirt"
 #config.vm.box_url = "https://download.fedoraproject.org/pub/fedora/linux/releases"\
 #                    "/32/Cloud/x86_64/images/Fedora-Cloud-Base-Vagrant-32-1"\
 #                    ".6.x86_64.vagrant-libvirt.box"
 config.vm.box = "centos/8"
 config.vm.box_version = "1905.1"


 # Forward traffic on the host to the development server on the guest.
 # You can change the host port that is forwarded to 5000 on the guest
 # if you have other services listening on your host's port 5000.
 config.vm.network "forwarded_port", guest: 2525, host: 2525

 # This is an optional plugin that, if installed, updates the host's /etc/hosts
 # file with the hostname of the guest VM. In Fedora it is packaged as
 # ``vagrant-hostmanager``
 if Vagrant.has_plugin?("vagrant-hostmanager")
     config.hostmanager.enabled = true
     config.hostmanager.manage_host = true
 end

 # Vagrant can share the source directory using rsync, NFS, or SSHFS (with the vagrant-sshfs
 # plugin). Consult the Vagrant documentation if you do not want to use SSHFS.
 config.vm.synced_folder ".", "/vagrant", disabled: true
 config.vm.synced_folder ".", "/home/vagrant/devel", type: "sshfs"

 # To cache update packages (which is helpful if frequently doing `vagrant destroy && vagrant up`)
 # you can create a local directory and share it to the guest's DNF cache. Uncomment the lines below
 # to create and use a dnf cache directory
 #
 # Dir.mkdir('.dnf-cache') unless File.exists?('.dnf-cache')

 # Comment this line if you would like to disable the automatic update during provisioning
 #config.vm.provision "shell", inline: "sudo apt update -y"
 #config.vm.provision "shell", inline: "sudo apt upgrade -y"
 config.vm.provision "shell", inline: "sudo dnf upgrade -y"

 # install useful packages and postfix dependencies
 config.vm.provision "shell", inline: "sudo dnf -y install git vim gcc gdb net-tools" #db*devel
 config.vm.provision "shell", inline: "sudo dnf -y groupinstall \"Development Tools\""
 config.vm.provision "shell", inline: "sudo dnf -y install postfix"
 config.vm.provision "shell", inline: "sudo dnf -y install libdb libdb-devel gcc openssl openssl-devel pcre pcre-devel openldap-devel cyrus-sasl cyrus-sasl-devel openldap postgresql postgresql-devel"
 #config.vm.provision "shell", inline: "sudo ln -s /usr/include/libdb4/db.h /usr/include/db.h"
 #config.vm.provision "shell", inline: "sudo ln -s /usr/include/libdb4/db.h /usr/include/db.h"

 # get the postfix source code
 config.vm.provision "shell", inline: "git clone https://github.com/jontrossbach/postfix.git"

 # make and install postfix
 config.vm.provision "shell", inline: "cd postfix/cf/ && sudo cp main.cf /etc/postfix/main.cf && sudo cp master.cf /etc/postfix/master.cf"
 config.vm.provision "shell", inline: "sudo -i && echo \"postfix:*:12345:12345:postfix:/no/where:/no/shell\" >> /etc/passwd && echo \"postfix:*:12345:\" >> /etc/group && echo \"postdrop:*:54321:\" >> /etc/group && exit"
 config.vm.provision "shell", inline: "cd /home/vagrant/postfix/postfix && sudo make"

 # need to figure out how to run this
 #config.vm.provision "shell", inline: "cd /home/vagrant/postfix/postfix && sudo make upgrade"
 config.vm.provision "shell", inline: "cd /home/vagrant/postfix/postfix && echo \"make makefiles CCARGS='-DHAS_PGSQL -I/usr/local/include/pgsql -fPIC -DUSE_TLS -DUSE_SSL -DUSE_SASL_AUTH -DUSE_CYRUS_SASL -DPREFIX=\\\"/usr\\\" -DHAS_LDAP -DLDAP_DEPRECATED=1 -DHAS_PCRE -I/usr/include/openssl -I/usr/include/sasl  -I/usr/include' AUXLIBS='-L/usr/local/lib -lpq -L/usr/lib64 -L/usr/lib64/openssl -lssl -lcrypto -L/usr/lib64/sasl2 -lsasl2 -lpcre -lz -lm -lldap -llber -Wl,-rpath,/usr/lib64/openssl -pie -Wl,-z,relro' OPT='-O' DEBUG='-g'\" > build-postfix.sh && chmod a+x build-postfix.sh" 
 config.vm.provision "shell", inline: "cd /home/vagrant/postfix/postfix && ./build-postfix.sh && make && make tidy && ./build-postfix.sh && make upgrade && sudo dnf -exclude postfix"
 
 # start network for postfix
 config.vm.provision "shell", inline: "sudo ifconfig eth0:1 127.0.0.1 netmask 255.255.255.255 up"

 # final steps to allow postfix to run permissionless
 config.vm.provision "shell", inline: "sudo chmod -R 777 /var/spool/postfix/ && sudo chmod -R 777 /var/lib/postfix"

 # Create the "postfix" box
 config.vm.define "postfix-2" do |postfix|
    postfix.vm.host_name = "postfix-dev.example.com"

    postfix.vm.provider :libvirt do |domain|
        # Season to taste
        domain.cpus = 4
        domain.graphics_type = "spice"
        domain.memory = 2048
        domain.video_type = "qxl"

        # Uncomment the following line if you would like to enable libvirt's unsafe cache
        # mode. It is called unsafe for a reason, as it causes the virtual host to ignore all
        # fsync() calls from the guest. Only do this if you are comfortable with the possibility of
        # your development guest becoming corrupted (in which case you should only need to do a
        # vagrant destroy and vagrant up to get a new one).
        #
        # domain.volume_cache = "unsafe"
    end
 end
end
