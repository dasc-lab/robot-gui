# DASC Robot GUI

[wip]

```
docker compose build
docker compose up
```

In the docker container:
```
cd /root/catkin_ws
colcon build
source install/setup.bash
```

launch rviz:
```
rviz2
```

and then in the toolbar, `Panels > Add New Panel > dasc_robot_gui > Teleop`

which will provide the gui inside RViz.
