// actions.js -- June 10, 2009
//    by geohot
// part of "The Embedded Disassembler"
//  released under GPLv3, see http://gplv3.fsf.org/

// We should be able to invalidate the cache somehow
var address_cache = {};

var address_view;   //last address is always + 0x30

function LoadFunction(address) {
  var xmlfunction = xx("READ", "/Function/"+immed(address), "").xml;
  graphDraw(xmlfunction.documentElement);
}


function send_reg_request() {
  var out = "";
  var state = xx("READ","/State","").xml;
  var registers = state.documentElement.getElementsByTagName("registers")[0];
  for(r in registers.childNodes) {
    if(registers.childNodes[r].childNodes != null)
      out += registers.childNodes[r].nodeName+": "+registers.childNodes[r].childNodes[0].nodeValue+"<br/>";
  }
  highlight(stoi("0x"+state.documentElement.getElementsByTagName("ProgramCounter")[0].childNodes[0].nodeValue));
  document.getElementById("registers").innerHTML=out;
}

var last_highlight = null;

function highlight(h) {
  if(last_highlight != null) {
    last_highlight.style.backgroundColor = "";
  }
  var a = document.getElementById(h);
  if(a==null) {
    LoadAreaFlat(h);
    last_highlight = document.getElementById(h);
    last_highlight.style.backgroundColor = "#CCCCCC";
  } else {
    a.style.backgroundColor = "#CCCCCC";
    last_highlight = a;
  }
}

function send_step_request() {
  if(selected.length == 1) {
    var ret = xx("STEP", "/Address/[`PC`]-4/"+selected[0].value, "");
  } else {
    var ret = xx("STEP", "/Address/[`PC`]-4", "");
  }
  //document.getElementById("response").value = view_xml(ret.xml.documentElement, 0);
  send_reg_request();
}

function LoadAddressFlat(address) {
  if(address_cache[address] == null) {
    //xx("DISASSEMBLE", "/Address/"+immed(address), "").xml;
    var ret = xx("READ", "/Address/"+immed(address), "").xml;
    address_cache[address] = ret;
  } else {
    var ret = address_cache[address];
  }

  //document.getElementById("flatfile").innerHTML += '<span class="address">'+ret.xml.getElementsByTagName("owner")[0].childNodes[0].nodeValue+": "+'</span>';

  var span = document.createElement("div");
  span.id = address;
  span.className = "address";

  var html = "";

  html += '<span class="expand">+</span>';

  html += '<span class="location_index"><span class="location_name">'+ret.documentElement.getElementsByTagName("name")[0].childNodes[0].nodeValue+'</span></span>';
  html += '<span class="data">'+ret.documentElement.getElementsByTagName("values")[0].lastChild.childNodes[0].nodeValue+'</span>';

  var pi = ret.documentElement.getElementsByTagName("ParsedInstruction");
  html += '<span class="instruction">';
  if(pi.length > 0) {
    html += ParsedInstructionToHTML(pi[0]);
  }
  html += "</span>"
  html += "<br/>";

  span.innerHTML = html;

  return span;
}

function LoadAreaFlat(start) {
  address_view = start;
  var i;
  var output = document.getElementById("flatfile");
  output.innerHTML = "";
  for(i=start; i < start+0x100; i+=2) {
    output.appendChild(LoadAddressFlat(i));
  }
}

window.addEventListener("load", function(e) { LoadAreaFlat(0x1800F998); }, false);

window.addEventListener("keypress", function(e) {
  if(e.keyCode == 112) {
    for(i in address_cache) {
      debug(i);
    }
  }
  //alert(e.keyCode);
  if(e.keyCode == 100 && selected.length > 0) {
    alert(immed(selected[0].value));
  }
  if(e.keyCode == 103 && selected.length > 0) {
    if(selected[0].type == "location")
      LoadAreaFlat(selected[0].value);
  }
  if(e.keyCode == 120 && selected.length > 0) {    //xrefs
    //alert(selected[0].value);
    var metadata = address_cache[selected[0].value].getElementsByTagName("metadata")[0];
    document.getElementById("xrefs").innerHTML = "";
    for(i in metadata.childNodes) {
      if(metadata.childNodes[i].nodeName == "input_indirect") {
        //debug(metadata.childNodes[i].childNodes[0].nodeValue);
        document.getElementById("xrefs").appendChild(LoadAddressFlat(stoi(metadata.childNodes[i].childNodes[0].nodeValue)));
      }
    }
  }
}, false);

window.addEventListener("dblclick", function(e) {
  if(selected[0].type == "location")
    LoadAreaFlat(selected[0].value);
}, false);

window.addEventListener("mousewheel", function(e) {
  //alert(e.wheelDelta/120);
  var file = document.getElementById("flatfile");
  if(e.wheelDelta < 0) {
    //alert(file.firstChild.nodeName);
    file.removeChild(file.firstChild);
    file.appendChild(LoadAddressFlat(address_view+0x100));
    address_view += 2;
  } else {
    address_view -= 2;
    file.insertBefore(LoadAddressFlat(address_view), file.firstChild);
    file.removeChild(file.lastChild);
  }
}, false);

window.addEventListener("click", function(e) {
  if(e.target.className == "expand" && e.target.innerHTML=="+") {  //plus sign clicked
    e.target.innerHTML = "-";
    var parent = e.target.parentNode;
    var ihtml = '<div class="expansion">';
    if(parent.className == "address") {
      ihtml += '<div class="changelistnumber"><span class="expand" style="margin-left: 30px">+</span>Stateless</div>';
      var xml = xx("READ", "/Address/"+parent.id+"/Owned","").xml.documentElement;
      for(i in xml.childNodes) {
        if(xml.childNodes[i].nodeName=="number") {
          var clnum = xml.childNodes[i].childNodes[0].nodeValue;
          ihtml += '<div class="changelistnumber"><span class="expand" style="margin-left: 30px">+</span>'+clnum+"</div>";
        }
      }
    } else if(parent.className == "changelistnumber") {
      if(parent.childNodes[1].nodeValue == "Stateless") {
        // Draw stateless changelist
        var changes = xx("READ", "/Address/"+parent.parentNode.parentNode.id+"/Instruction/StatelessChangelist").xml.documentElement;
        ihtml += '<div class="changelist" style="margin-left: 60px">';
        for(i in changes.childNodes) {
          if(changes.childNodes[i].nodeName == "change") {
            ihtml += '<span class="changetarget">' + changes.childNodes[i].childNodes[0].childNodes[0].nodeValue + '</span>'
            ihtml += '<span class="changecondition">(' + changes.childNodes[i].childNodes[1].childNodes[0].nodeValue + ')</span>'
            ihtml += "=";
            ihtml += '<span class="changesource">' + changes.childNodes[i].childNodes[3].childNodes[0].nodeValue + '</span><br/>'
          }
        }
        ihtml += '</div>';
      } else {
        ihtml += '<div class="changelist" style="margin-left: 60px">';
        var xml = xx("READ", "/Changelist/"+parent.childNodes[1].nodeValue,"").xml.documentElement;
        var changes = xml.getElementsByTagName("changes")[0];
        for(i in changes.childNodes) {
          if(changes.childNodes[i].nodeName == "change") {
            ihtml += '<span class="changetarget">' + changes.childNodes[i].childNodes[0].childNodes[0].nodeValue + '</span>'
            ihtml += "=";
            ihtml += '<span class="changesource">' + changes.childNodes[i].childNodes[1].childNodes[0].nodeValue + '</span><br/>'
          }
        }
        ihtml += '</div>';
      }
    }


    ihtml += '</div>';
    parent.innerHTML += ihtml;
  } else if(e.target.className == "expand" && e.target.innerHTML=="-") {
    e.target.innerHTML = "+";
    var parent = e.target.parentNode;
    parent.removeChild(parent.lastChild);
  }
}, false);
