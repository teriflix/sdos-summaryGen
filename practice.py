import re
with open("/Users/teo/Desktop/Big-Fish.fountain.txt") as file:
    script=file.read()

from transformers import pipeline
import torch
from screenplay_tools.fountain.parser import Parser
fp= Parser()
fp.add_text(script)
text=fp.script.dump()
# print(text[:5000])
p1=r'ACTION:"([\S\s]*?)"'
r1=r'(ACTION) \1'

p2=r'HEADING:"(.*)"'
r2=r'### Scene: \1'

p3=r'CHARACTER:"(.*)"'
r3=r'\1:'

p4=r'DIALOGUE:"(.*)"'
r4=r'"\1"'
text=re.sub(p1,r1,text)
text=re.sub(p2,r2,text)
text=re.sub(p3,r3,text)
text=re.sub(p4,r4,text)

inds=[]
to_find="### Scene: "
for i in range(len(text)):
    flag=True
    for j in range(len(to_find)):
        if (i+j<len(text) and text[i+j]==to_find[j]):
            pass
        else:
            flag=False
            break
    if (flag):
        inds.append(i)

scenes=[]
i=0
for j in inds:
    if (text[i:j]):
        scenes.append(text[i:j])
    i=j
if (text[i:]):
    scenes.append(text[i:])

print(len(scenes))
# for m in match:
#     print(m)