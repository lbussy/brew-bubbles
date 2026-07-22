FROM python@sha256:18b8bae1ea34d6c9cef6f29833c9d0913cea3d5c6d98a1bd2db2037348473450

RUN apt-get update \
    && apt-get install --yes --no-install-recommends \
        ca-certificates \
        git \
    && rm -rf /var/lib/apt/lists/*

COPY requirements.txt /tmp/requirements.txt

RUN python -m pip install --no-cache-dir \
        pip==23.3.2 \
        setuptools==69.0.3 \
        wheel==0.42.0 \
    && python -m pip install --no-cache-dir \
        --requirement /tmp/requirements.txt \
    && rm /tmp/requirements.txt

RUN python --version \
    && git --version \
    && pio --version
