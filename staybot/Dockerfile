FROM alpine:edge
RUN apk add --update --no-cache build-base clang make libstdc++ libgcc
WORKDIR /app
COPY . .
RUN make
CMD ./StayBot.exe
