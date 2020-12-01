postfix/AAAREADME

This is a for of Postfix MTA which aims to run without root prveledges inside a container enviroment. The Docker image provided was tested in OpenShift using the `new-app` command as follows:

```$ oc new-app https://github.com/jontrossbach/postfix.git```

Along with [this guide](https://devops.ionos.com/tutorials/configure-a-postfix-relay-through-gmail-on-centos-7/) for setting up postfix as an smtp relay.

Some users may find it more managable to test their postfix changes in the vagrant vm before testing on OpenShift so to run an instance of unpriveledged Postfix you should set the `-DUSE_UNPRIV` compiler directive [here in the Vagantfile](https://github.com/jontrossbach/postfix/blob/master/Vagrantfile#L70). To get the Vagrant machine running from the repository root directiory run:

```$ vagrant up```
