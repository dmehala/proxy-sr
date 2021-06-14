import sys
import os
import re
import argparse
import requests
import logging
import urllib.parse
from http.server import BaseHTTPRequestHandler, HTTPServer
from http.client import parse_headers
from typing import Any, List

logging.getLogger("requests").setLevel(logging.WARNING)
logging.getLogger("urllib3").setLevel(logging.WARNING)


# Helper
uri_attribute_pattern = re.compile('^#.*uri=(.*)', re.IGNORECASE)


def remove_quotes(s: str) -> str:
    return s[1:-1]


def get_all_uris(s: str) -> List[str]:
    uris: List[str] = list()

    for l in s.split('\n'):
        l = l.strip()
        if l:
            if l.startswith('#'):
                m = uri_attribute_pattern.search(l)
                if m:
                    uris.append(remove_quotes(m.group(1)))
            else:
                uris.append(l)
    
    return uris


def sanitize_uri(uri: str) -> str:
    if not uri.startswith('http'):
        uri = uri.replace('../', '')
    return '/' + uri


def resolve_uri(curr_manifest_path, uri):
    base, _ = curr_manifest_path.rsplit('/', 1)
    base += '/'
    return urllib.parse.urljoin(base, uri)


class MITMProxyRequestHandler(BaseHTTPRequestHandler):
    protocol_version = 'HTTP/1.0'
    # default_uri = 'https://bitdash-a.akamaihd.net/content/sintel/hls/playlist.m3u8'
    # default_uri = 'https://bitdash-a.akamaihd.net/content/MI201109210084_1/m3u8s/f08e80da-bf1d-4e3d-8899-f0f6155f6efa.m3u8'
    # default_uri = 'http://wowza-test.streamroot.io/liveOriginAbsolute/_definst_/sintel-live.smil/chunklist_b1128000.m3u8'
    default_uri = 'https://mnmedias.api.telequebec.tv/m3u8/29880.m3u8'
    curr_manifest = default_uri
    session = requests.Session()
    uris_cache = list()
    
    def log_message(self, format: str, *args: Any) -> None:
        return


    def pp_path(self) -> str:
        return f'http://{self.server.server_address[0]}:{self.server.server_address[1]}{self.path}'


    def forward_data(self, uri, is_manifest):
        r = self.session.get(uri)
        # r.raise_for_status()

        # Headers
        self.send_response(r.status_code)
        # for k, v in r.headers.items():
        #     self.send_header(k, v)

        # Overwrite content-length 'cause we do not support gzip
        self.send_header('Content-Encoding', '')
        self.send_header('Content-Length', str(len(r.content)))
        self.end_headers()

        if r.status_code == 200:
            if is_manifest:
                # uris = get_all_uris(r.content.decode())
                self.uris_cache += [resolve_uri(self.curr_manifest, x) for x in get_all_uris(r.content.decode())]
                # for x in uris:
                #     key = sanitize_uri(x)
                #     self.uris_cache[key] = resolve_uri(self.curr_manifest, x)
                # self.uris_cache.update({ sanitize_uri(x):resolve_uri(self.curr_manifest, x) for x in uris })
                # for k, v in self.uris_cache.items():
                #     print(k,v)

                # DBG
                # _, filename = os.path.split(uri)
                # with open(os.path.join('dbg', filename), 'w') as f:
                #     f.write(r.content.decode())

            self.wfile.write(r.content)
            self.wfile.flush()

        return r.elapsed.total_seconds() * 1000


    def make_uri(self) -> str:
        if not self.path or self.path == '/':
            return self.default_uri

        for x in self.uris_cache:
            if self.path in x:
                return x
        # return self.uris_cache.get(self.path, '')
        return ''


    def do_GET(self):
        # TODO: pass headers from VLC -> Requests
        # print(self.headers.keys())
        # print(parse_headers(self.headers))
        kind = {
            '.ts': 'SEGMENT',
            '.m3u': 'MANIFEST',
            '.m3u8': 'MANIFEST',
            '.vtt': 'SUBTITLE'
        }

        uri = self.make_uri()
        _, ext = os.path.splitext(uri)
        ctx = kind[ext]

        if ctx == 'MANIFEST' and self.curr_manifest != uri:
            print('[TRACK SWITCH]')
            self.curr_manifest = uri

        print('[IN][{}]  {}'.format(ctx, self.pp_path()))
        print(f'URI generated: {uri}')
        elapsed_ms = self.forward_data(uri, ctx=='MANIFEST')
        # elapsed_ms = 0
        print('[OUT][{}]  {} ({:.0f}ms)'.format(ctx, self.pp_path(), elapsed_ms))

        self.close_connection = True


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Proxy')
    parser.add_argument('uri', help='URI to request')
    args = parser.parse_args()

    server_addr = ('127.0.0.1', 8081)
    server = HTTPServer(server_addr, MITMProxyRequestHandler)

    print(f'Server is up and running at http://{server_addr[0]}:{server_addr[1]}')
    server.serve_forever()

    sys.exit(0)