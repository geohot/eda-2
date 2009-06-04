function simulator_step_request() {
  var ret2 = xx("READ", "/Address/[`PC`]-8/Instruction/Parsed", "");
  var ret = xx("STEP", "/Address/[`PC`]-8", "");

  document.getElementById("instructions").innerHTML += '<span class="address">'+ret.xml.getElementsByTagName("owner")[0].childNodes[0].nodeValue+": "+'</span>';
  document.getElementById("instructions").innerHTML += ParsedInstructionToHTML(ret2.xml.documentElement);
}
