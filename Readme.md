# ESP32GithubOTA

This library makes it possible to use Github as a host for your firmware.

### Requirements
1. `firmware.bin` file needs to be in the root folder of your repository.
2. `info.txt` file needs to be in the root as well containing the `md5` hash of the formware at the start.

### Example
```
root
  |- firmware.bin
  |- info.txt
  |- other stuff
  |- ...
```

### info.txt content
I generate the md5 with the `md5` command like this `md5 firmeware.bin > info.txt`.
```
02a4caf5aa544c9bd7c8fa68650ffef7  ./firmware.bin
```

### Easiest way using Github Actions
With this actin you simple check out your repository, build it, moves all the needed files to the necessary places and pushes the files to a release branch.
```
name: build and deploy

on: [push]

jobs:
  build:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v1
        with:
          fetch-depth: 0
      - name: Set up Python
        uses: actions/setup-python@v1
      - name: Install dependencies
        run: |
          python -m pip install --upgrade pip
          pip install platformio
      - name: Run PlatformIO
        run: |
          platformio run
          mv .pio/build/default/firmware.bin ./firmware.bin
          md5sum ./firmware.bin > info.txt                  
      - name: push
        uses: actions-x/commit@v2
        with:
          email: you@your.domain
          rebase: true
          force: true
          name: GitHub Actions Autocommitter
          branch: release
          files: .
```
