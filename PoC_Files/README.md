# How to run PoC

## Prerequisites

    - Linux based OS (Tested on debian 12)
    - Docker
    - Python3
    - gcc


## Steps to Replicate
In the shell of your choice run:

```shell
git clone https://github.com/BirdsAreFlyingCameras/GPT-5_Jailbreak_PoC
```
```shell
cd GPT-5_Jailbreak_PoC
```
```shell
python3 -m venv venv
```
```shell
source venv/bin/activate
```
```shell
python3 server.py
```
<br/>

In a new shell window:
```shell
gcc agent.c -o agent
```
```shell
chmod +x agent
```
```shell
./agent
```
<br/>

In a another new shell window: 
```shell
docker build -t traffic-sink .
```
```shell
docker run --rm -it -p 8100:8100 -e SINK_PORT=8100 -e SINK_PROTO=udp traffic-sin
```
<br/>

Now in a web browser open panel.html and launch your attack via the panel.
