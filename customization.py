import requests
import config
import utils


s = requests.Session()

s.post(f"http://{config.TARGET}/wp-login.php",
       headers={'Cookie': 'wordpress_test_cookie=WP Cookie check'}, data={
        "log": "shou",
        "pwd": "shou@123",
        "wp-submit": "Log In",
        "redirect_to": config.TARGET,
        "testcookie": "1"})

REQUEST_SESSION = s
CUSTOM_MUTATION = []


def handler(result):
    return 1


RESP_HANDLER = handler
