## Порядок установки и настройки виртуального кластера disco

0. Пройти аутентификацию в ОС виртуальной машины: login - general, password - qwerty.
1. Настроить сетевое соединение между узлами кластера
   1.1. Настроить статический ip-адресс на сетевой карте "VMware Network Adapter VMnet8" (NAT).
   1.2. Настроить статические ip-адреса на виртуальных машинах
      1.2.1. В файле /etc/network/interfaces закоментировать строку
         ```
         # iface ens33 inet dhcp
         ```
      1.2.2. Добавить в файл /etc/network/interfaces строки (можно использовать текстовый редактор nano)
         ```
         auto ens33
            iface ens33 inet static
            address 192.168.xxx.xxx
            netmask 255.255.255.0
            gateway 192.168.xxx.xxx
            dns-nameservers 192.168.xxx.xxx
         ```
      1.2.3. Перезапустить службу networking
         ```
         sudo ifdown ens33
         sudo ifup ens33
         ```
      1.2.4. Проверить произведенную настройку
         ```
         ifconfig
         ```
      1.2.5. Проверить наличие соединения по ip-адресу с другими узлами сети 
         ```
         ping 192.168.xxx.xxx
         ```
   1.3. Настроить идентификацию узлов сети по именам
      1.3.1. Добавить в файл /etc/hosts слеующие строки
         ```
         192.168.xxx.xx0 node00
         192.168.xxx.xx1 node01
         192.168.xxx.yyy realhost 
         ```
      1.3.2. Проверить наличие соединения по именам с другими узлами сети 
         ```
         ping <имя узла>
         ```
      1.3.3. В файле /etc/hostname изменить имя компьютера на nodeXX.
      1.3.4. Перезагрузить ОС
         ```
         sudo shutdown -r now
         ```
      1.3.5. Проверить заданное имя компьютера
         ```
         hostname
         ```
2. Создать пользователя disco
   2.1. Создать папку 
      ```
      sudo mkdir /srv/disco
      ```
   2.2. Создать группу пользователей disco
      ```
      sudo groupadd disco
      ```
   2.3. Создать пользователя disco
      ```
      sudo useradd -d /srv/disco -g disco -s /bin/bash disco
      ```
   2.4. Владельцем папки /srv/disco установить пользователя disco
      ```
      sudo chown disco:disco /srv/disco
      ```
3. Настроить беспарольное ssh-соединение между узлами кластера от имени пользователя disco.
   3.1. На каждом вычислительном узле для пользователя disco сгенерировать ключевую пару
      ```
      sudo -u disco ssh-keygen -N '' -f /srv/disco/.ssh/id_dsa
      ```
   3.2. Скопировать открытую часть ключа в папку /home/general/keys
      ```
      mkdir ~/keys
      sudo cp /srv/disco/.ssh/id_dsa.pub /home/general/keys
      sudo chown general:general ~/keys/id_dsa.pub
      ```
   3.3. Переименовать скопированный файл к имени, совпадающим с именем узла
      ```
      mv ~/keys/id_dsa.pub ~/keys/nodexx.pub
      ```
   3.4. Скопировать файл открытой части ключа на остальные узлы кластера
      ```
      scp ~/keys/nodexx.pub general@nodeyy:/home/general/keys
      ```
   3.5. Записать содержимое всех файлов открытых ключей в файл /srv/disco/authorized_keys
      ```
      sudo sh -c "cat /home/general/keys/nodexx.pub >> /srv/disco/.ssh/authorized_keys"
      ```
   3.6. Установить пользователя disco владельцем файла /srv/disco/.ssh/authorized_keys
   3.7. На каждом узле от имени пользователя disco выполнить беспарольный вход на каждый узел кластера (на вопрос ввести слово "yes").
      ```
      sudo -u disco ssh nodexx
      ```
4. Установить программное обеспечение из папки /home/general/distr
   4.1. Проверить наличие запущенной службы sshd
      ```
      ps -ax | grep sshd
      ```
   4.2. Используя утилиту pscp загрузить на виртуальную машину файлы deb-пакетов необходимого программного обеспечения
      ```
      pscp <путь к папаке с дистрибутивами> general@192.168.xxx.xxx:/home/general
      ```
   4.3. Установка deb-пакета
      ```
      sudo dpkg -i <имя файла>.deb
      ```
   4.4. Проверка наличия установленного deb-пакета в ОС
      ```
      dpkg -l | grep <имя пакета(не файла)>
      ```
5. Установить программное обеспечение disco
   5.1. Сборка дистрибутива disco
      ```
      make
      cd lib
      python setup.py build
      ```
   5.2. На вычеслительном узле master
      ```
      sudo make install
      cd lib
      sudo python setup.py install
      ```
   5.3. На вычислительном узле woker
      ```
      sudo make install-core
      sudo make install-node
      cd lib
      sudo python setup.py install
      ```
   5.4. Изменить владельца каталога /usr/var/disco на пользователя disco
      ```
      sudo chown -R disco:disco /usr/var/disco
      ```
   5.5. Добавить в файл /etc/disco/settings.py строки
      ```
      DISCO_USER = "disco"
      DISCO_JOB_OWNER = "disco"
      ```
6. Настроить взаимодействие на уровне библиотек erlang:
   6.1. На всех вычислительных узлах в папке /srv/disco создать файл .erlang.cookie, содержащий строку из символов английского алфавита и симполов подчёркивания
      ```
      sudo -u disco sh -c "echo DISCO_CLUSTER_ERLANG_COOKIE_STRING >> /srv/disco/.erlang.cookie"
      ```
   6.2. Владельцем файла должен быть пользователь disco:disco
   6.3. Права доступа к файлу установить 400
7. Запустить кластер disco и провериь его работоспособность
   7.1. Ввести команду
      ```
      sudo disco start
      ```
   7.2. В браузере зайти по адресу 192.168.<адрес мастера>:8989
   7.3. Выбрать страницу "configure"
   7.4. В таблице добавить имена всех настроенных узлов кластера node00, node01, ...
   7.5. Вернуться на страницу "ststus".
   7.6. Убедиться в корректном соединении со всеми узлами кластера.
   7.7. Запустить тестовую программу на кластере.
   
      
      

