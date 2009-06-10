// util.js -- June 10, 2009
//    by geohot
// part of "The Embedded Disassembler"
//  released under GPLv3, see http://gplv3.fsf.org/

function immed(data) {
  if(data < 10)
    return data.toString(10);
  else
    return "0x"+data.toString(16);
}

function stoi(data) {
  return parseInt(data);
}

function debug(str) {
  var a = document.getElementById("debug");

  if(a != null) {
    a.innerHTML += str + "<br/>";
  }
}

function ParsedInstructionToHTML(pi_element) {
  //console.debug(pi_element);
  var ret = '<span class="opblock">';
  var first_formatting = true;
  for(i in pi_element.childNodes) {
    if(pi_element.childNodes[i].nodeName == "atom") {
      if(pi_element.childNodes[i].childNodes[1].childNodes[0] != null) {
        ret += '<span class="'+
          pi_element.childNodes[i].childNodes[0].childNodes[0].nodeValue+
          '">'+
          pi_element.childNodes[i].childNodes[1].childNodes[0].nodeValue+
          '</span>';
      }
    } else if(pi_element.childNodes[i].nodeName == "formatting") {
      if(first_formatting == true) {
        ret+='</span>';
        first_formatting = false;
      }
      ret += pi_element.childNodes[i].childNodes[0].nodeValue;
    }
  }
  if(first_formatting == true) {
    ret+='</span>';
    first_formatting = false;
  }
  return ret;
}
