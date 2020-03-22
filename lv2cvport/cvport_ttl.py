import json
import rdflib
import sys
from rdflib import URIRef, Literal
from pathlib import Path

bin_path = sys.argv[1]
json_path = sys.argv[2]
plugin_name = Path(json_path).parent.stem

ttl_path = f"{bin_path}/{plugin_name}.lv2/{plugin_name}_dsp.ttl"

if not Path(ttl_path).exists():
    # print(f"{plugin_name} is not built.")
    exit(0)

graph = rdflib.Graph()
result = graph.parse(ttl_path, format="turtle")

plugin = URIRef(f"https://github.com/ryukau/LV2Plugins/tree/master/{plugin_name}")
lv2port = URIRef("http://lv2plug.in/ns/lv2core#port")
lv2index = URIRef("http://lv2plug.in/ns/lv2core#index")
lv2symbol = URIRef("http://lv2plug.in/ns/lv2core#symbol")
lv2name = URIRef("http://lv2plug.in/ns/lv2core#name")
lv2CVPort = URIRef("http://lv2plug.in/ns/lv2core#CVPort")
lv2AudioPort = URIRef("http://lv2plug.in/ns/lv2core#AudioPort")
rdfType = URIRef("http://www.w3.org/1999/02/22-rdf-syntax-ns#type")

with open(json_path, "r") as json_file:
    port_json = json.load(json_file)

for subject, port in graph.subject_objects(lv2port):
    if subject != plugin:
        continue
    index: str = "index" + str(graph.value(port, lv2index))
    if index in port_json:
        port_data = port_json[index]
        graph.add((port, rdfType, lv2CVPort))
        graph.remove((port, rdfType, lv2AudioPort))
        graph.set((port, lv2symbol, Literal(port_data["symbol"])))
        graph.set((port, lv2name, Literal(port_data["name"])))

serialized = graph.serialize(format="turtle")
with open(ttl_path, "wb") as fi:
    fi.write(serialized)
