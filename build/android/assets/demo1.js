var json = [
	["rect", {"x":1, "y":20, "width":100, "height":"127", "fill":0xff610, "opacity":1, "id":"test2"},],
	["image", {"x":121, "y":20, "width":850, "height":672, "src":"res:///image001.png", "id":"test1"},],
	["path", {"d":"M20 20 C90 40 130 40 180 20 S250 60 280 20", "fill":"none", "stroke":"#0000ff"}],
	["text", {"x":1, "y":50, "width":100, "height":"127", "fill":0xff610, "opacity":1, "id":"test4", "font-size":55}, "ha哈ha"],
];

function SvgInit() {
	::print("SvgInit");
//	return;
	
	var alpha = 0.01;
	setInterval(function () {
		alpha += 0.01;
		if (alpha >= 1)
			alpha = 0.1;
		$("test1").attr({"opacity":alpha});
	}, 100);
}
	
loadScriptResourceAndParseJson([], json, svgRoot());