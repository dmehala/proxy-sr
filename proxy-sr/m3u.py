import os
import re
from typing import List
# Accept-Ranges bytes
# Access-Control-Expose-Headers Date, Server, Content-Type, Content-Length
# Cache-Control max-age=1
# Access-Control-Allow-Origin *
# Access-Control-Allow-Credentials true
# Access-Control-Allow-Methods OPTIONS, GET, POST, HEAD
# Connection Keep-Alive
# Content-Encoding gzip
# Access-Control-Allow-Headers Content-Type, User-Agent, If-Modified-Since, Cache-Control, Range
# Date Mon, 14 Jun 2021 22:10:07 GMT
# Content-Type application/vnd.apple.mpegurl
# Content-Length 268
# playlist = 'chunklist_b1128000.m3u8'
playlist = 'f08e80da-bf1d-4e3d-8899-f0f6155f6efa.m3u8'

def remove_quotes(s):
    return s[1:-1]

uri_attribute_pattern = re.compile('^#.*uri=(.*)', re.IGNORECASE)
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

content = ""

with open(os.path.join('dbg', playlist)) as f:
    for l in f:
        # print(l)
        content += l

print(get_all_uris(content))

# base = r"https://www.mathworks.com/help/pdf_doc/install/index.html"
# link_in_html = r"../matlab/licensing.pdf"

# result = urllib.parse.urljoin(base, link_in_html)