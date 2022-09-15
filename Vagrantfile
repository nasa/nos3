Vagrant.configure("2") do |config|
    # Uncomment one of the following to select configuration
    config.vm.box = "nos3/ubuntu"
    
    # Specify version
    config.vm.box_version = "0.0.0"
    
    # Share host NOS3 repository into VM
    config.vm.synced_folder ".", "/home/nos3/Desktop/github-nos3"

    # General configuration
    config.ssh.password = "vagrant"
    config.vm.provider "virtualbox" do |vbox|
        vbox.name = "nos3_0.0.0"
        vbox.gui = true
        vbox.cpus = 2
        vbox.memory = "8192"
    end
end
