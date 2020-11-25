## ������� ��������� � ��������� ������������ �������� disco

0. ������ �������������� � �� ����������� ������: login - general, password - qwerty.
1. ��������� ������� ���������� ����� ������ ��������
   1.1. ��������� ����������� ip-������ �� ������� ����� "VMware Network Adapter VMnet8" (NAT).
   1.2. ��������� ����������� ip-������ �� ����������� �������
      1.2.1. � ����� /etc/network/interfaces ��������������� ������
         ```
         # iface ens33 inet dhcp
         ```
      1.2.2. �������� � ���� /etc/network/interfaces ������ (����� ������������ ��������� �������� nano)
         ```
         auto ens33
            iface ens33 inet static
            address 192.168.xxx.xxx
            netmask 255.255.255.0
            gateway 192.168.xxx.xxx
            dns-nameservers 192.168.xxx.xxx
         ```
      1.2.3. ������������� ������ networking
         ```
         sudo ifdown ens33
         sudo ifup ens33
         ```
      1.2.4. ��������� ������������� ���������
         ```
         ifconfig
         ```
      1.2.5. ��������� ������� ���������� �� ip-������ � ������� ������ ���� 
         ```
         ping 192.168.xxx.xxx
         ```
   1.3. ��������� ������������� ����� ���� �� ������
      1.3.1. �������� � ���� /etc/hosts �������� ������
         ```
         192.168.xxx.xx0 node00
         192.168.xxx.xx1 node01
         192.168.xxx.yyy realhost 
         ```
      1.3.2. ��������� ������� ���������� �� ������ � ������� ������ ���� 
         ```
         ping <��� ����>
         ```
      1.3.3. � ����� /etc/hostname �������� ��� ���������� �� nodeXX.
      1.3.4. ������������� ��
         ```
         sudo shutdown -r now
         ```
      1.3.5. ��������� �������� ��� ����������
         ```
         hostname
         ```
2. ������� ������������ disco
   2.1. ������� ����� 
      ```
      sudo mkdir /srv/disco
      ```
   2.2. ������� ������ ������������� disco
      ```
      sudo groupadd disco
      ```
   2.3. ������� ������������ disco
      ```
      sudo useradd -d /srv/disco -g disco -s /bin/bash disco
      ```
   2.4. ���������� ����� /srv/disco ���������� ������������ disco
      ```
      sudo chown disco:disco /srv/disco
      ```
3. ��������� ������������ ssh-���������� ����� ������ �������� �� ����� ������������ disco.
   3.1. �� ������ �������������� ���� ��� ������������ disco ������������� �������� ����
      ```
      sudo -u disco ssh-keygen -N '' -f /srv/disco/.ssh/id_dsa
      ```
   3.2. ����������� �������� ����� ����� � ����� /home/general/keys
      ```
      mkdir ~/keys
      sudo cp /srv/disco/.ssh/id_dsa.pub /home/general/keys
      sudo chown general:general ~/keys/id_dsa.pub
      ```
   3.3. ������������� ������������� ���� � �����, ����������� � ������ ����
      ```
      mv ~/keys/id_dsa.pub ~/keys/nodexx.pub
      ```
   3.4. ����������� ���� �������� ����� ����� �� ��������� ���� ��������
      ```
      scp ~/keys/nodexx.pub general@nodeyy:/home/general/keys
      ```
   3.5. �������� ���������� ���� ������ �������� ������ � ���� /srv/disco/authorized_keys
      ```
      sudo sh -c "cat /home/general/keys/nodexx.pub >> /srv/disco/.ssh/authorized_keys"
      ```
   3.6. ���������� ������������ disco ���������� ����� /srv/disco/.ssh/authorized_keys
   3.7. �� ������ ���� �� ����� ������������ disco ��������� ������������ ���� �� ������ ���� �������� (�� ������ ������ ����� "yes").
      ```
      sudo -u disco ssh nodexx
      ```
4. ���������� ����������� ����������� �� ����� /home/general/distr
   4.1. ��������� ������� ���������� ������ sshd
      ```
      ps -ax | grep sshd
      ```
   4.2. ��������� ������� pscp ��������� �� ����������� ������ ����� deb-������� ������������ ������������ �����������
      ```
      pscp <���� � ������ � ��������������> general@192.168.xxx.xxx:/home/general
      ```
   4.3. ��������� deb-������
      ```
      sudo dpkg -i <��� �����>.deb
      ```
   4.4. �������� ������� �������������� deb-������ � ��
      ```
      dpkg -l | grep <��� ������(�� �����)>
      ```
5. ���������� ����������� ����������� disco
   5.1. ������ ������������ disco
      ```
      make
      cd lib
      python setup.py build
      ```
   5.2. �� �������������� ���� master
      ```
      sudo make install
      cd lib
      sudo python setup.py install
      ```
   5.3. �� �������������� ���� woker
      ```
      sudo make install-core
      sudo make install-node
      cd lib
      sudo python setup.py install
      ```
   5.4. �������� ��������� �������� /usr/var/disco �� ������������ disco
      ```
      sudo chown -R disco:disco /usr/var/disco
      ```
   5.5. �������� � ���� /etc/disco/settings.py ������
      ```
      DISCO_USER = "disco"
      DISCO_JOB_OWNER = "disco"
      ```
6. ��������� �������������� �� ������ ��������� erlang:
   6.1. �� ���� �������������� ����� � ����� /srv/disco ������� ���� .erlang.cookie, ���������� ������ �� �������� ����������� �������� � �������� �������������
      ```
      sudo -u disco sh -c "echo DISCO_CLUSTER_ERLANG_COOKIE_STRING >> /srv/disco/.erlang.cookie"
      ```
   6.2. ���������� ����� ������ ���� ������������ disco:disco
   6.3. ����� ������� � ����� ���������� 400
7. ��������� ������� disco � �������� ��� �����������������
   7.1. ������ �������
      ```
      sudo disco start
      ```
   7.2. � �������� ����� �� ������ 192.168.<����� �������>:8989
   7.3. ������� �������� "configure"
   7.4. � ������� �������� ����� ���� ����������� ����� �������� node00, node01, ...
   7.5. ��������� �� �������� "ststus".
   7.6. ��������� � ���������� ���������� �� ����� ������ ��������.
   7.7. ��������� �������� ��������� �� ��������.
   
      
      

