from seleniumwire import webdriver
from seleniumrequests.request import RequestMixin
from selenium.webdriver.chrome.options import Options


class CustomWebDriver(webdriver.Chrome, RequestMixin):
    pass


def get_selenium_obj():
    chrome_options = Options()
    chrome_options.headless = True
    chrome_options.add_argument("--no-sandbox")
    chrome_path = r'/usr/bin/chromedriver'
    driver = CustomWebDriver(executable_path=chrome_path, options=chrome_options)
    return driver


def done_using_selenium_obj(obj):
    obj.close()
    obj.quit()


