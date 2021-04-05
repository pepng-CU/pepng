FROM emscripten/emsdk:2.0.16

WORKDIR /src

ENTRYPOINT ["tail"]
CMD ["-f","/dev/null"]