// selection.js -- June 10, 2009
//    by geohot
// part of "The Embedded Disassembler"
//  released under GPLv3, see http://gplv3.fsf.org/

// selected is global and accessible by all things
var selected = [];

window.addEventListener("mousedown", function(e) {
// deselect all
  var good = false;
  var node = e.target.parentNode;
  while(node != null) {
    if(node.id == "flatfile") {
      good = true;
      break;
    }
    node = node.parentNode;
  }

  if(good == false) return false;
  if(e.ctrlKey == false) {
    var i;
    for(i in selected) {
      selected[i].colored.style.backgroundColor = "";
    }
    selected.length = 0;
  }

  if(e.target.className=="location_name") {
    //alert(address_cache[e.target.parentNode.id]);
    e.target.style.backgroundColor = "orange";
    if(e.target.parentNode.className=="location_index") {
      selected.splice(0, 0, {type: "location", value: stoi(e.target.parentNode.parentNode.id), colored: e.target} );
    } else {
      selected.splice(0, 0, {type: "location_name", value: e.target.innerHTML, colored: e.target} );
    }
  }

  if(e.target.className=="location") {
    e.target.style.backgroundColor = "orange";
    selected.push( {type: "location", value: stoi(e.target.innerHTML), colored: e.target} );
  }

  e.preventDefault();
  return false;
}, false);
