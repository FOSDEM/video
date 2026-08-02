from setuptools import setup

setup(name='video_status',
      version='1',
      description='Daemon for updating the status on the video box display',
      url='https://github.com/FOSDEM/video',
      packages=['video_status'],
      install_requires=[
          'pyserial',
      ],
      entry_points={
          'console_scripts': ['video-status=video_status.__main__:main'],
      })