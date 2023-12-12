Vagrant.configure("2") do |config|
    ###
    ### Notes:
    ###   Using the provided base boxes is not required to use the software
    ###   Base boxes are provisioned from the following location
    ###   * https://github.com/nasa-itc/deployment
    ###   Links to which commit was used is captured in the box release notes
    ###   

    ### Uncomment one of the following to select configuration
    #config.vm.box = "nos3/rocky" # Not yet updated to support
    config.vm.box = "nos3/ubuntu"
    
    ### Specify version
    config.vm.box_version = "20231101"
    
    ### Share host NOS3 repository into VM
    config.vm.synced_folder ".", "/home/jstar/Desktop/github-nos3", 
        owner: 'jstar', group:'vboxsf', automount:'true', 
        mount_options: ["dmode=0770", "fmode=0770"]

    ### General configuration
    config.vm.provider "virtualbox" do |vbox|
        vbox.name = "nos3_20231101"
        vbox.gui = true
        ### Enable additional configuration as needed
        vbox.cpus = 4
        vbox.memory = "8192"
    end
end
