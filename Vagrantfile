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

    ### VM Setup
    config.vm.define "sv" do |sv|
    end

    config.vm.define "fsw", autostart: false, primary: false do |fsw|
        fsw.vm.network :private_network, ip: "10.0.0.101"
    end

    config.vm.define "gsw", autostart: false, primary: false do |gsw|
        gsw.vm.network :private_network, ip: "10.0.0.102"
    end

    config.vm.provider "virtualbox" do |vb|
        vb.gui = true
        vb.cpus = 4
        vb.memory = "8192"
    end
end
