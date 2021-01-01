# HSFuzz

A whitebox coverage-guided {Fuzzer, Crawler, Explorer} for PHP Apps.

### Download
```bash
git clone https://github.com/shouc/hsfuzz.git && cd hsfuzz
```

### Installation (Docker)
**Build instrumented PHP Docker image:**


Assume you have docker installed and Dockerfile for your web app ready.

```bash
cd ext && docker build . -t php7-instr
```

Replace your Dockerfile's `FROM phpblablabla` with `FROM php7-instr:latest`


Add following line to Dockerfile to create a cov dir at the public html directory of your web app. 

```dockerfile
RUN mkdir cov && chmod 777 cov
```

**Python Stuffs:**


```bash
pip3 install -r requirements-docker.txt
```


### Installation (Direct)
**PHP Stuffs:**


Assume you have php7.x-dev installed and using Linux.

```bash
cd ext
phpize && ./configure
make && make install
# restart PHP, change below
sudo service php7.x-fpm restart
```

You can then use `phpinfo()` to check whether hsfuzz is installed. 

If it is not installed, edit the fpm or whatever you are using's `php.ini` to include the
so file of the plugin and restart PHP. It is likely located in `ext/modules/hsfuzz.so`. 

Then run `mkdir cov && chmod 777 cov` to create a cov dir at the public html directory of your web app. 

**Python Stuffs:**
```bash
pip3 install -r requirements.txt
```


### Run
Edit config.py and run 
```bash
python3 start.py
```

