Vagrant.configure("2") do |config|
    ### Uncomment one of the following to select configuration
    #config.vm.box = "nos3/rocky"
    config.vm.box = "nos3/ubuntu"
    
    ### Specify version
    config.vm.box_version = "20230731"
    
    ### Share host NOS3 repository into VM
    config.vm.synced_folder ".", "/home/nos3/Desktop/github-nos3", 
        owner: 'root', group:'vboxsf', automount:'true', 
        mount_options: ["dmode=0770", "fmode=0770"]

    ### General configuration
    config.vm.provider "virtualbox" do |vbox|
        vbox.name = "nos3_20230731"
        vbox.gui = true
        ### Enable additional configuration as needed
        vbox.cpus = 6
        vbox.memory = "6144"
    end
end
