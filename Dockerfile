FROM ros:foxy

ARG DEBIAN_FRONTEND noninteractive 

RUN apt-get update && apt-get install -y --no-install-recommends vim 

RUN apt-get install -y --no-install-recommends qt5-default

RUN apt-get install -y --no-install-recommends ros-${ROS_DISTRO}-rviz2

SHELL ["/bin/bash", "-c"]

RUN echo "source /opt/ros/${ROS_DISTRO}/setup.bash" >> ~/.bashrc
