// eda_core.js -- July 23, 2009
//    by geohot
// part of "The Embedded Disassembler"
//  released under GPLv3, see http://gplv3.fsf.org/

function toHexByte(i) {
  i &= 0xFF;
  if(i<0x10)
    var ret = "0";
  else
    var ret = "";
  ret += i.toString(16).toUpperCase();
  return ret;
}

// same functionality as C immed
function immed(i) {
  if(i<10) return i.toString(16);
  else return "0x"+i.toString(16).toUpperCase();
}

function toHexByteLittleEndian(i, len) {
  var j;
  var ret = "";
  for(j=0;j<len;j++) {
    ret += toHexByte(i);
    if((j+1) != len)
      ret += " ";
    i >>= 8;
  }
  return ret;
}

function eda_init() {
  var i, test;
  for(i=4194304;i<4194304+0x60;) {
    test = xx("READ", "/Address/"+i, "");
    //test = eval('({"hello": "hi", "die": 45})');
    document.getElementById("cb").appendChild(MakeDOMAddress(test.Address));
    i+=test.Address.size;
  }
  window.console.log("read done: "+test.Address.gai);
}

function MakeDOMAddress(addr) {
  var ret = document.createElement("div");
  ret.className = "Address";
  ret.gai = addr.gai;
  var id = document.createElement("span");
  id.className = "addresslocation";
  if(addr.name != null) {
    id.innerHTML = addr.name;
  } else {
    id.innerHTML = addr.location;
  }
  ret.appendChild(id);

  var lastdata = addr.values[addr.values.length-1].data;
  var value = document.createElement("span");
  value.className = "value";
  value.innerHTML = toHexByteLittleEndian(lastdata, addr.size);
  ret.appendChild(value);
  
  if(addr.Instruction != null)
    ret.appendChild(MakeDOMParsedInstruction(addr.Instruction.ParsedInstruction));
  else if(addr.type == "data") {
    var data = document.createElement("span");
    data.className = "immed";
    data.innerHTML = immed(lastdata);
    ret.appendChild(data);
  }
    
  return ret;
}

function MakeDOMParsedInstruction(pi) {
  var ret = document.createElement("span");
  ret.className = "ParsedInstruction";
  var opblock = document.createElement("span");
  opblock.className = "opblock";
  for(atom in pi) {
    thisatom = document.createElement("span");
    thisatom.className = pi[atom].type;
    thisatom.innerHTML = pi[atom].data;
    if(opblock != null) {
      opblock.appendChild(thisatom);
      if(thisatom.className == "formatting") {
        ret.appendChild(opblock);
        opblock = null;
      }
    } else {
      ret.appendChild(thisatom);
    }
  }
  return ret;
}