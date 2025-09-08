FROM python:3.11-slim

WORKDIR /app

COPY traffic_sink.py /app/

RUN pip install --no-cache-dir psutil

CMD ["python", "traffic_sink.py"]
