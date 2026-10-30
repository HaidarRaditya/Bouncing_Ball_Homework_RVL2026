import os
from launch import LaunchDescription
from launch_ros.actions import Node
from ament_index_python.packages import get_package_share_directory

def generate_launch_description():
    cfg = os.path.join(get_package_share_directory('bouncing_ball_catcher'), 'config', 'params.yaml')
    return LaunchDescription([
        Node(package='bouncing_ball_catcher', executable='camera_node', parameters=[cfg]),
        Node(package='bouncing_ball_catcher', executable='game_node', parameters=[cfg]),
    ])