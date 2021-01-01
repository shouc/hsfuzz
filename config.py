import re
import docker

USE_DOCKER = False
# Don't Use Docker
TARGET = "localhost"
HOST_NAMES_CARED = [TARGET]

# Use Docker
DOCKER_INSTANCE = docker.from_env()
DOCKER_FILE_LOC = ""

# Config for Crawling
REDIS_HOST = "localhost"
REDIS_PORT = 6379
WAIT_TIME = 1
IGNORE_LINKS = [re.compile("logout")]
DONT_CARE_STATUS_CODE = [404, 502, 500, 403]

def handler(result):
    return 1


RESP_HANDLER = handler
