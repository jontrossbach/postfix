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
 config.vm.box = "peru/ubuntu-18.04-server-amd64"
 config.vm.box_version = "20200806.01"
 #config.vm.box = "f32-cloud-libvirt"
 #config.vm.box_url = "https://download.fedoraproject.org/pub/fedora/linux/releases"\
                     #"/32/Cloud/x86_64/images/Fedora-Cloud-Base-Vagrant-32-1"\
                     #".6.x86_64.vagrant-libvirt.box"

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
 config.vm.provision "shell", inline: "sudo apt update -y"
 config.vm.provision "shell", inline: "sudo apt upgrade -y"
 #config.vm.provision "shell", inline: "sudo dnf upgrade -y"

 # install useful packages and postfix dependencies
 config.vm.provision "shell", inline: "sudo apt -y install libdb-dev git vim build-essential m4 gdb net-tools"

 # get the postfix source code
 config.vm.provision "shell", inline: "git clone https://github.com/jontrossbach/postfix.git"

 # make and install postfix
 config.vm.provision "shell", inline: "cd postfix/cf/ && sudo mkdir /etc/postfix && sudo cp main.cf /etc/postfix/main.cf && sudo cp master.cf /etc/postfix/master.cf"
 config.vm.provision "shell", inline: "sudo -i && echo \"postfix:*:12345:12345:postfix:/no/where:/no/shell\" >> /etc/passwd && echo \"postfix:*:12345:\" >> /etc/group && echo \"postdrop:*:54321:\" >> /etc/group && exit"
 config.vm.provision "shell", inline: "cd /home/vagrant/postfix/postfix && sudo make"
 config.vm.provision "shell", inline: "cd /home/vagrant/postfix/postfix && sudo make upgrade"

 # start network for postfix
 config.vm.provision "shell", inline: "sudo ifconfig eth0:1 127.0.0.1 netmask 255.255.255.255 up"

 # final steps to allow postfix to run permissionless
 config.vm.provision "shell", inline: "sudo chmod -R 777 /var/spool/postfix/ && sudo chmod -R 777 /var/lib/postfix"

 # bootstrap and run with ansible
 #config.vm.provision "shell", inline: "sudo apt -y install mailutils"


 # Create the "postfix" box
 config.vm.define "postfix" do |postfix|
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
