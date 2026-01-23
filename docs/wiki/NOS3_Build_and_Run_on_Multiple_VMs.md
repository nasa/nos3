# NOS3 Building and Running on Multiple Machines

When running NOS3, it is possible to split it across multiple VMs or physical computers such that the ground software is on one computer and the satellite simulators on another one. This is done as follows:

## VM Setup

First it is necessary to have two VMs or physical machines which can communicate with each other. For VMs, this requires activating a second network card via settings:

![image](https://github.com/user-attachments/assets/68ca0685-139d-45bc-9c3d-8ac31837dcc0)

While the VM is powered off, the 'Enable Network Adapter' box can be checked and the VM itself attached to a NAT Network (which allows for communication between different VMs on the same host, unlike the default NAT adapter). If no NAT Network yet exists, one can be created under 'Tools' in VirtualBox.

In the case of physical machines, both should be on the same network and able to ping each other.

## GSW Computer/VM

Firstly, it is necessary to create a 'manager' and 'worker' node in a Docker swarm. We will not be using any of the swarm containers or launch mechanisms, just the networking side of things. I have presently decided to make the VM which runs the ground software (COSMOS) the 'manager', and so it should be possible to prepare it merely by running

        make prep-gsw

This command sets a static IP address and initializes the swarm, with the VM on which it is run being set as the 'manager' node. It will provide in the output a command which must be run on the other VM; since the 'swarm join' commands include a randomly assigned token, that could not be hardcoded. Then, once that has been run, merely running

        make start-gsw

should launch the ground software (COSMOS) and create the relevant overlay networks. The overlay networks are exactly the same (in name) as the existing bridge networks; the only difference is that they should - in theory - be able to extend over multiple VMs.

Strictly speaking, it is not always necessary to run `make prep-gsw`; merely running

        docker swarm init --advertise-addr <your_ip_address>

will also work if the computers are on the same network.

#Satellite Computer/VM

As mentioned above, the output from `make prep-gsw` will give a command which must be run on the satellite computer/VM in order to attach it to the swarm networks. This command will be in the form of

        docker swarm join --token <your_token> <your_ip_address>:2377

Where <your_ip_address> will probably be 10.10.10.101 (but if not is going to be 10.10.10.100). The command will be supplied in the output to `make prep-gsw` or `docker swarm init --advertise-addr <your_ip_address>` and will need to be run after running

        make prep-sat

if running after `make prep-gsw` to ensure that the IP address is properly set and the computers will be able to talk to one another. Then, to run the satellite side of things, simply run

        make start-sat

This will have to be run after `make start-gsw`, because it does not create the required networks.

#Troubleshooting

Once both VMs have been added to a NAT network, the aforementioned instructions for the ground software VM/computer should work. If they do not, however, it may be necessary to add the network interface "eth1" to each computer by editing the netplan files under /etc/netplan and running `sudo netplan apply`. The necessary changes are as follows (with the satellite computer having an IP address of 10.10.10.101 and the GSW computer having an IP address of 10.10.10.100):

![image](https://github.com/user-attachments/assets/14b733f1-3d04-4c4a-8209-9b5004bfec44)

One additional point - if the ground software fails to load properly (especially after it has been started, stopped, and then started again), rebuilding it and relaunching it should solve the problem.
