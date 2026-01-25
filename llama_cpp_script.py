from transformers import pipeline
import torch
from screenplay_tools.fountain.parser import Parser
import re

with open("/Users/teo/Desktop/Big-Fish.fountain.txt") as file:
    script=file.read()

#Parsing the script using screenplay fountain parser
fp= Parser()
fp.add_text(script)
text=fp.script.dump()

#Converting json elements into machine understandable tokens
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

#Splitting the scenes as the model can not understand very big contexts
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

model_id="/Users/teo/Desktop/sdos/Llama-3.2-3B-Instruct"

pipe = pipeline(
    "text-generation",
    model=model_id,
    model_kwargs={"dtype": torch.bfloat16},
    device="mps",  
)

messages = [
    {
        "role": "system",
        "content": '''You are an expert Script Coverage Specialist. Your task is to read a pre-formatted movie script segment and generate a concise, continuous narrative summary.

### INPUT FORMAT GUIDE
The user will provide text that has been converted into a readable script format:

1. SCENE HEADERS: Marked with "### Scene:"
   - Example: "### Scene: INT. BEDROOM - NIGHT (1973)"
   - INSTRUCTION: Use these to track the flow of time and location. If the year changes (e.g., 1973 to 1987), explicitly mention "Years later..." or the specific year in your summary.

2. ACTION LINES: Marked with "(Action) ...]"
   - Example: "(Action) Edward takes the ring off."
   - INSTRUCTION: This is the primary source of physical plot events. Treat these as definitive facts.

3. DIALOGUE: Format is CHARACTER: "Speech"
   - Example: EDWARD (V.O.): "I caught the fish."
   - INSTRUCTION: Do not transcribe quotes. Summarize the *intent* of the conversation.
   - Note: "(V.O.)" means Narration. Treat this as the storyteller speaking to the audience, distinct from the physical scene.

### WRITING RULES
- Write a single, cohesive paragraph in the present.
- Focus on the conflict: What does the protagonist want? What stands in their way?
- Differentiate between the "Story being told" (Edward's tall tales) and the "Reality" (Will watching him tell it).
- Do not use bullet points or list formatting.'''
    },
   {
            "role": "user", 
            "content": f'''Here is the script segment to summarize:\n{scenes[0]}\nWrite the narrative summary now.'''
            }
]


prev_summary=""
summaries=[]
i=0

#Grouping scenes such that their total characters is <2500 and putting summaries in summaries[]
while (i<len(scenes)):
 
    # if (i!=0):
    #     curr=""
    #     while (len(curr)<2500 and i<len(scenes)):
    #         curr+=scenes[i]
    #         i+=1
    #     this_scene=f"PREVIOUSLY IN THE STORY: {prev_summary}\n\n CURRENT SCENE TEXT: {curr}"
    # else:
    curr=""
    while (len(curr)<2500 and i<len(scenes)):
        curr+=scenes[i]
        i+=1
    this_scene=curr

    messages[1]={
            "role": "user", 
            "content": f'''Here is the script segment to summarize:\n{this_scene}\nWrite the narrative summary now.'''
            }
    outputs = pipe(
        messages,
        max_new_tokens=300,
        do_sample=False,
        temperature=None, 
        top_p=None
    )
    # print(this_scene)
    # print("-------------------------------------------------")
    
    prev_summary=outputs[0]["generated_text"][-1]["content"]
    print(prev_summary)
    print("-------------------------------------------------")
    print("-------------------------------------------------")
    print("-------------------------------------------------")
    print()
    summaries.append(prev_summary)
    
