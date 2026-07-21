# Brew Bubbles Documentation

This folder contains the source files for the Brew Bubbles documentation.  If you are seeking to view the documentation you may do so [here](https://brew-bubbles.readthedocs.io/en/latest/).

The site is built with Sphinx and MyST Markdown. From this directory, build it
locally in an isolated environment:

```shell
python3 -m venv .venv
.venv/bin/python -m pip install -r requirements.txt
.venv/bin/sphinx-build -W --keep-going -b html . _build/html
```
