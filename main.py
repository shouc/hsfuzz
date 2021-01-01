import time
import requests
import config
import customization
import mutator
import utils
import docker_utils
from urllib.parse import urlparse, ParseResult, urlunparse
from bs4 import BeautifulSoup
import re
import cov
import prior


def link_to_url(link: str) -> str:
    # discard domain, fragment, and
    url_obj = urlparse(link)
    if url_obj.scheme not in ["http", "https", ""]:
        # not the protocol we care
        return ''
    if url_obj.hostname and url_obj.hostname not in config.HOST_NAMES_CARED:
        return ''
    return urlunparse(ParseResult(
        scheme='',
        netloc='',
        path=url_obj.path,
        params=url_obj.params,
        query=url_obj.query,
        fragment=''))


def should_ignore_link(link: str):
    for l in config.IGNORE_LINKS:
        if l.match(link):
            return True
    return False


def extract_links(content: str) -> [str]:
    soup = BeautifulSoup(content, features="lxml")
    for link in soup.findAll('a', attrs={'href': re.compile(".+?")}):
        link = link_to_url(link["href"])
        if link == '':
            continue
        if not utils.REDIS_OBJ.hexists("already_crawled", link) and not should_ignore_link(link):
            crawl_with_eval(link)


def get_link_from_redis():
    url = utils.REDIS_OBJ.srandmember("url_seed")
    if not url:
        return None
    return url


def crawl_link(link: str):
    utils.REDIS_OBJ.hset("already_crawled", link, "1")
    url = f'http://{docker_utils.get_target()}{link}'
    utils.DEBUG(url)
    cov_uuid = cov.get_cov_header()
    result = customization.REQUEST_SESSION.get(url, headers={
        "Cov-Loc": cov_uuid
    })
    extract_links(result.text)
    return result, cov_uuid


def crawl_with_eval(link):
    result, cov_uuid = crawl_link(link)
    if result.status_code in config.DONT_CARE_STATUS_CODE:
        return False
    config.RESP_HANDLER(result)

    has_new_cov = cov.evaluate_cov(cov_uuid)
    # priority = prior.evaluate_prior(result)
    if has_new_cov:
        utils.REDIS_OBJ.sadd("url_seed", link)
        return True
    return False


def main(name):
    # init crawl
    utils.INFO("hsfuzz started")
    crawl_link("")
    print("Init finished")
    cov_count = 0
    while 1:
        link = get_link_from_redis()
        while link is None:
            utils.INFO("Waiting for links...")
            link = get_link_from_redis()
            time.sleep(config.WAIT_TIME)
        link = link.decode("latin-1")
        m = mutator.Mutator(link)
        m.mutate()
        link = m.to_url()
        if utils.REDIS_OBJ.hexists("already_crawled", link):
            continue
        utils.DEBUG(f"Working on link {link}")
        crawl_with_eval(link)



