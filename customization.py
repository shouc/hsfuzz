import requests
import config
import utils
import hashlib
import os
import time


s = requests.Session()

# s.post(f"http://{config.TARGET}/wp-login.php",
#        headers={'Cookie': 'wordpress_test_cookie=WP Cookie check'}, data={
#         "log": "shou",
#         "pwd": "shou@123",
#         "wp-submit": "Log In",
#         "redirect_to": config.TARGET,
#         "testcookie": "1"})

REQUEST_SESSION = s
CUSTOM_MUTATION = []


def handler(result):
    md5 = hashlib.md5()
    md5.update(result.text.encode("utf-8"))
    os.system(f"echo \"{md5.hexdigest()}\" >> benchmark/page/{time.time()}.txt")
    return 1


RESP_HANDLER = handler
