# webserv
42 School - Level05

HTTP webserver written in C++98

## Info

As the server uses `kqueue` as an event notification interface, the server can only be run on [certain operating systems](https://en.wikipedia.org/wiki/Kqueue).  
The HTTP/1.1 is implemented to a certain extend as in [RFC9110](https://datatracker.ietf.org/doc/html/rfc9110).
Usable methods are GET, HEAD, POST, PUT and DELETE.
The configuration file is oriented towards [nginx](https://nginx.org/en/docs/).

The default error pages are located in `error/`.  
Two different CGI scripts are present in `cgi/`.  
The `db_business/` and `tests/` folders are for evaluation purposes only.  
The `conf/` folder contains some preset configuration files.  

## Usage

`make` compiles the program.

The first and only argument has to be the configuration file of the server
```
$ ./webserv <configuration.file>
```
By default the `conf/server.conf` will be used.

The configuration file has some requirements:

+ Multiple servers per file are possible. (_server_)
+ Choose the host of each ’server’. (_host_)
+ Choose the port(s) of each ’server’. (_listen_)
+ Setup the server_names or not. (_server_name_)
+ Setup default error pages. (_error_page_)
+ Limit client body size. (_client_max_body_size_)
+ Setup routes with one or multiple of the following rules: (_route_)
  + Define a list of accepted HTTP methods for the route. (_method_)
  + Define a HTTP redirection. (_redirect_)
  + Define a directory or a file from where the file should be searched. (_root_)
  + Turn on or off directory listing. (_directory_listing_)
  + Set a default file to answer if the request is a directory. (_index_)
  + Execute certain CGI based on certain file extension for spefific methods. (_cgi_ext_, _cgi_method_, _cgi_pass_)
  + Make the route able to accept uploaded files and configure where they should be saved. (_upload_)
 
E.g.:
configuration.file
```
server {
  host                    127.0.0.1;
  listen                  8080;
  listen                  8082;

  server_name             my_server my.server;

  error_page 400          error/default.html;

  client_max_body_size    10000;

  route / {
    method                GET POST PUT DELETE;
    root                  /some/directory;
    directory_listing     on;
    upload                on;
  }

  route /other {
    method                GET HEAD;
    root                  /other/directory;
    index                 index.html;
    cgi_method            GET POST;
    cgi_ext               .php;
    cgi_pass              cgi/php-cgi;
  }

  route /redirect/ {
    method                GET;
    redirect              /other;
  }
}

server {
  host      127.0.0.1;
  listen    8081;

}
```
