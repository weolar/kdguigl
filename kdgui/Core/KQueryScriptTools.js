//file:///F:\KxE\kis_released_sp7.4_1337_fb\tools\kdgui3\kdgui\Core\KQueryScriptTools.js

function doParseJson(lazyTask, jsonStr, parent) {
	for (var i = 0; i < jsonStr.len(); ++i)
		doParseJsonArray(lazyTask, jsonStr[i], parent);
}

function parseJsonAndCall(jsonStr, parent) {
    var lazyTask = {"kdcall":[]};
    doParseJson(lazyTask, jsonStr, parent);
    for (var j = 0; j < lazyTask.kdcall.len(); ++j)
        lazyTask.kdcall[j].func(lazyTask.kdcall, lazyTask.kdcall[j].node);
}

function doParseJsonArray(lazyTask, jsonNode, parent) {
    var jsonNodeType = ::type(jsonNode);
	if ("function" == jsonNodeType) {
	    jsonNode(parent);
	    return;
    } else if ("null" == jsonNodeType) {
        return;
    } else if ("instance" == jsonNodeType && "kdcall" in jsonNode) {
        jsonNode.kdcall(parent);
        return;
	}
	
	var key;
	var len = jsonNode.len();
	var nodeInst = null;
	var nodeName = jsonNode[0];
	var attrList = jsonNode[1];
	var txtStr = null;
	
	if (2 > len || "string" != ::type(nodeName))
	    return;
	    
    var key, value;
	var attrListCopy = {};
	var bIsControl = null != nodeName.find(".");
	if (!bIsControl) // 如果是控件
	    nodeInst = $$(nodeName);
	    
	var kdcallFunc = null;
	foreach(key, value in attrList) {
	    var valueType = ::type(value);
		if ("string" == valueType) {
			attrListCopy[key] <- value;
		} else if ("function" == valueType && "kd-call" == key) {
		    kdcallFunc = value;
		    if (!bIsControl)
			    lazyTask.kdcall.append({"node":nodeInst, "func":kdcallFunc});
		} else 
		    attrListCopy[key] <- value;
    }
    
	if (bIsControl) { // 如果是控件
	    var controlDes = split(nodeName, ".");
	    if (2 != controlDes.len())
	        return;
        var controler = getroottable()[controlDes[0]]()[controlDes[1]](parent, attrListCopy, null);
        if (kdcallFunc)
            lazyTask.kdcall.append({"node":controler, "func":kdcallFunc});
        return;
	}

    nodeInst.attr(attrListCopy);
    attrListCopy = null;
	
	nodeInst.appendTo(parent);
	for (var i = 2; i < len; ++i) {
	    var jsonNodeItType = ::type(jsonNode[i]);
		if ("array" == jsonNodeItType) {
			doParseJsonArray(lazyTask, jsonNode[i], nodeInst);
		} else if ("string" == jsonNodeItType) {
			if (!txtStr)
			    txtStr = "";
			txtStr += jsonNode[i];
		}
	}
	if (null != txtStr)
		nodeInst.text(txtStr);
}

function centerRect(n, x, y, w, h, W, H) {n.attr(["x", (x+w - W)/2, "y", (y+h - H)/2, "width", W, "height",H]);}
function centerX(n, x, y, w, h, W, H) {n.attr(["x", (x+w - W)/2, "y", y, "width", W, "height",H]);}

function __parseJson__(jsonStr, rootNode) {
	var lazyTask = {"kdcall":[],"controlExpand":[]};
	doParseJson(lazyTask, jsonStr, rootNode);
				
	var j = 0;
	for (j = 0; j < lazyTask.kdcall.len(); ++j)
	    lazyTask.kdcall[j].func(lazyTask.kdcall, lazyTask.kdcall[j].node);

	for (j = 0; j < lazyTask.controlExpand.len(); ++j)
	    KwExpandToWidget(lazyTask.controlExpand[j]);
	
	SvgInit();
	__KwSvgInit__();
}

// "kdcall" "Control Expand" "resizeLayout"
function loadScriptResourceAndParseJson(scriptSrcQueue, jsonStr, rootNode) {
	var loadConut = 0;
	if (0 == scriptSrcQueue.len()) {
		__parseJson__(jsonStr, rootNode);
		return;
	}
	for (var i = 0; i < scriptSrcQueue.len(); ++i) {
	    if (!KLoadSyncScriptFromSrc(scriptSrcQueue[i]))
	        return
// 		$$("script").attr(["externalResourcesRequired","true","src",scriptSrcQueue[i]]).appendTo((rootNode)).bind("load",function(evt,data){
// 			loadConut++;
// 			if (loadConut == scriptSrcQueue.len()) {
// 				__parseJson__(jsonStr, rootNode);
// 			}
// 		}, false, null);
	}
	
	__parseJson__(jsonStr, rootNode);
}