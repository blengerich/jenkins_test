* [Install Docker](https://docs.docker.com/engine/installation/mac/)
* Create account for [DockerHub](https://hub.docker.com/)
* Send username to me (@blengerich)
Once you have been added to the project:
* Open Docker Quickstart Terminal for Mac
* Pull the docker image:
   $ docker login
   $ docker pull blengerich/genamap
* To check that it worked:
   $ docker run -ti blengerich/genamap
To run the node app:
* From the root directory (Genamap_V2), run:
   $ docker run -ti -p 49160:3000 -v ${PWD}:/usr/src/genamap blengerich/genamap
* This will create a local image of the docker container. Now we will need to run the node app:
  $ cd /usr/src/genamap/src/frontend/genamapApp
  $ nodemon -L webapp.js
* To view the website, from a different terminal window, run:
  $ docker-machine ip default
* Since we are running on a Mac, Docker binds localhost to the VM that the image is running on. This means that we need the VM's ip to see the actual site. Now, with the ip address, go to <ip>:49160, where <ip> is probably something like '192.168.99.100/' in your browser
If you are changing files in genamapApp/src (i.e., editing the node source code), you will need to open another instance of the same docker container.
* From the same terminal window as the docker-machine command, run:
  $ docker ps -a
* This will show you a list of your active containers. The first one should be the currently running one. Note the name of this container (last column). In a third terminal window, run:
  $ docker exec -ti <container_name> /bin/bash
* This will open another interactive terminal for that container. Now go to the main node directory:
  $ cd /usr/src/genamap/src/frontend/genamapApp
* Now we will 'gulp' the files (i.e., update them), by running:
  $ gulp watch