// eda_core.js -- July 23, 2009
//    by geohot
// part of "The Embedded Disassembler"
//  released under GPLv3, see http://gplv3.fsf.org/

/*function eda_init() {
  var i, test;
  for(i=4194304;i<4194304+0x60;) {
    test = xx("READ", "/Address/"+i, "");
    document.getElementById("cb").appendChild(MakeDOMAddress(test.Address));
    i+=test.Address.size;
  }
  window.console.log("read done: "+test.Address.gai);
}*/

function eda_init() {
  fetchAddress(0x400000);

  window.addEventListener("keypress", eventKeyHandler, false);
  window.addEventListener("mousedown", eventClickHandler, false);
  window.addEventListener("dblclick", eventDblClickHandler, false);
  
  document.getElementById("cb").focus();
  
}

function fetchAddress(addr) {
  var i, test;
  var box = document.getElementById("cb");
  
  box.innerHTML = "";
  
  test = xx("READ", "/Address/"+addr, "");
  box.appendChild(expansionAllow(makeDOMAddress(test.Address)));
  
  var top = test.Address.location;
  
  for(i=top+(test.Address.size);i<top+0x80;) {
    test = xx("READ", "/Address/"+i, "");
    box.appendChild(expansionAllow(makeDOMAddress(test.Address)));
    i+=test.Address.size;
  }
}

// ***** event handler functions *****

function eventKeyHandler(e) {
  var s = String.fromCharCode(e.keyCode).toLowerCase();
  //alert(s);
  if(s == "g") {
    address = stoi(prompt("Address", ""));
    fetchAddress(address);
  } else if(s == "e") {
    toeval = prompt("Eval", "");
    test = xx("EVAL", "/"+toeval, "");
    alert(test.result);
  }
}

// selection
var selection = [];

function eventClickHandler(e) {
  var t = e.target;
  while((selected = selection.pop()) != null) {
    selected.style.backgroundColor = "";
  }
  
  if(t.className == "location") {
    t.style.backgroundColor = "orange";
    selection.push(t);
  }
  e.preventDefault();
  //return false;
}

function eventDblClickHandler(e) {
  var t = e.target;
  if(selection.indexOf(t) != -1) {
    t.style.backgroundColor = "red";
    fetchAddress("`"+e.target.innerHTML+"`");
  }
  //alert(e.target.innerHTML);
}

// ***** expansion functions *****

function expansionAllow(obj) {
  var expand = document.createElement("span");
  expand.innerHTML = "+";
  expand.onclick = expansionHandler;
  expand.className = "expand";
  obj.insertBefore(expand, obj.firstChild);
  return obj;
}

function expansionHandler(obj) {
  var nextNode = obj.target.parentNode.childNodes[1];
  if(nextNode.className == "baseaddress") {
    var address = nextNode.childNodes[0].innerHTML;
    window.console.log("click: "+address);
  }
  
  //window.console.log("clicked: "+nextNode.className);
}

// ***** formatting functions *****

function makeDOMAddress(addr) {
  var ret = document.createElement("div");
  ret.className = "Address";
  ret.gai = addr.gai;
  var id = document.createElement("span");
  id.className = "baseaddress";
  
  var id_address = document.createElement("span");
  id_address.className = "location";
  
  if(addr.name != null) {
    id_address.innerHTML = addr.name;
  } else {
    id_address.innerHTML = addr.location;
  }
  id.appendChild(id_address);
  ret.appendChild(id);

  var lastdata = addr.values[addr.values.length-1].data;
  var value = document.createElement("span");
  value.className = "value";
  value.innerHTML = toHexByteLittleEndian(lastdata, addr.size);
  ret.appendChild(value);
  
  if(addr.Instruction != null)
    ret.appendChild(makeDOMParsedInstruction(addr.Instruction.ParsedInstruction));
  else if(addr.type == "data") {
    var data = document.createElement("span");
    data.className = "immed";
    data.innerHTML = immed(lastdata);
    ret.appendChild(data);
  }
    
  return ret;
}

function makeDOMParsedInstruction(pi) {
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

// ***** low-level formatting functions *****

// same functionality as C immed
function immed(i) {
  if(i<10) return i.toString(16);
  else return "0x"+i.toString(16).toUpperCase();
}

function stoi(s) {
  return parseInt(s);
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

function toHexByte(i) {
  i &= 0xFF;
  if(i<0x10)
    var ret = "0";
  else
    var ret = "";
  ret += i.toString(16).toUpperCase();
  return ret;
}
