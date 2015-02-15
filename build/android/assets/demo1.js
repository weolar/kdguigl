var json = [
	["rect", {"x":1, "y":20, "width":100, "height":"127", "fill":0xff1100, "opacity":1, "id":"test1"},],
	["path", {"d":"M20 20 C90 40 130 40 180 20 S250 60 280 20", "fill":"none", "stroke":"#0000ff"}],
];

function SvgInit() {
	::print("SvgInit");
	return;
	
	var alpha = 0.01;
	setInterval(function () {
		alpha += 0.01;
		if (alpha >= 1)
			alpha = 0.01;
		$("test1").attr({"opacity":alpha});
	}, 100);
}
loadScriptResourceAndParseJson([], json, svgRoot());