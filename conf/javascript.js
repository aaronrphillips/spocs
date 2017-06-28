var base_dir       = "";
var columns        = new Object();
var defaultColumns = new Object();
var slider_range;
var fasta_data;
var fasta_data_nav;
var fasta_data_text;


function SelectAllData()
{    
    fasta_data_text.focus();
    fasta_data_text.select();

    return;
}


function show_fasta_data()
{
    var ids = Object.keys(fasta);
    var html = "";
  
    for (var i in ids)
    {
        var id  = ids[i];
        var seq = fasta[id].replace(/__NL__/g, "\n");
        html   += seq + "\n";
    } 

    fasta_data_text.innerHTML =  html;
    fasta_data.style.display  = "block";

    return;
}


function common_body_load()
{
    base_dir = document.getElementsByTagName('base')[0].href;
    console.log("basedir:" + base_dir + ":");
    var head = document.getElementsByTagName('head')[0];

    var link = document.createElement('link');
    link.rel = "shortcut icon";
    link.href = base_dir + "include/images/MaximalClique.ico";
    head.appendChild(link);

    var div = document.createElement('div');
    div.innerHTML = '<div id="floater" style="display:none; position:absolute; top:100; left:100; width:32; height:102; background-color:white;">Floater</div>';
    document.body.appendChild(div);
  
    var div2 = document.createElement('div');
    div2.innerHTML = '<div id="color_scale" style="position:absolute; display:none; top:50; left:50; width:250px; height:132px;  margin-top:-120px; background-color:white;"></div>';
    document.body.appendChild(div2);

    fasta_data       = document.createElement('div');
    fasta_data.id    = "fasta_data";
    fasta_data.class = "fasta_data";
    fasta_data_nav   = document.createElement('div');
    fasta_data_nav.id = "fasta_data_nav";
    fasta_data_nav.class = "fasta_data_nav";
    fasta_data_nav.innerHTML = '<button onclick="SelectAllData()">Select All</button><button onclick="javascript:fasta_data.style.display = ' + "'none'" + '">Close</button>';
    fasta_data_text   = document.createElement('textarea');
    fasta_data_text.id = "fasta_data_text";
    fasta_data_text.class = "fasta_data_text";
    fasta_data_text.cols  = 61;
    fasta_data_text.rows  = 25;
    fasta_data_text.wrap  = "soft";
    
    fasta_data.appendChild(fasta_data_nav);
    fasta_data.appendChild(fasta_data_text);
    document.body.appendChild(fasta_data);
  
    var props = getProperties();
    var col_array = Object.keys(props);
    for (var c in col_array)
    {
        var col = col_array[c];
        defaultColumns[col] = new Object();
        defaultColumns[col]['min'] = props[col]['min'];
        defaultColumns[col]['max'] = props[col]['max'];
    }
  
}


function clique_body_load()
{
    common_body_load();
    do_clique();
    fill_dynamic_form();
    fill_dynamic_metadata_table();
}


function subgraph_body_load()
{
    common_body_load();
    do_subgraph(false);
    fill_dynamic_form();
    fill_dynamic_metadata_table();
}


function get_base_dir()
{
    return base_dir;
}


function format_number(value)
{
    if (value != null && value.toString().match(/\./g))
    {
        value = value.toFixed(3);
    }

    return value;
}


function toggle_column(column)
{
    if (columns[column])
    {
        delete columns[column];
    }
    else
    {
        columns[column] = 1;
    }
  
    color_slices_by_metadata(Object.keys(columns));
}


function cancel_range(column)
{
    document.getElementById('color_scale').style.display = "none";
}


function apply_range(column)
{
    var min = $("#left_slider_value").val();
    var max = $("#right_slider_value").val();
    document.getElementById('color_scale').style.display = "none";
  
    var props = getProperties();
    props[column]['min'] = Number(min);
    props[column]['max'] = Number(max);
  
    color_slices_by_metadata(Object.keys(columns));
}


function get_perc(i, min_perc, max_perc, zero_loc)
{

    var perc = i / 100.0;

    // green side
    if (i > zero_loc)
    {
        // past max (brightest green)
        if (i > (zero_loc + (max_perc / 2)))
        {
            perc = .99;
        }
        else
        {
            perc = ((i - zero_loc) / max_perc) + .5;
        //console.log(i + ":" + perc + ":" + max_perc);
        }
    }
    if (i < zero_loc)
    {
        if (i < (100 - (zero_loc + (min_perc / 2))))
        {
            perc = .01;
        }
        else
        {
            perc = .5 - ((zero_loc-i) / min_perc);
        //console.log(i + ":" + perc + ":" + min_perc);
        }
    }
  
    return perc;
}


function draw_gradient(orientation, orig_min, orig_max, min, max, value)
{
    var max_perc  = (max / orig_max) * 100;
    var min_perc  = (Math.abs(min) / Math.abs(orig_min)) * 100;
    var dimension = 'width:30px; height:2px;';
    var html      = '<table style="margin-left:auto; margin-right:auto; border: 1px solid white;" cellspacing="0" cellpadding="0">';

    if (orientation == "horizontal") 
    {
        html += '<tr>';
        dimension = 'width:2px; height:30px;';
    }

    for (var i = 1; i <= 99; ++i)
    {
        var zero_loc = 100 - ((orig_max / (orig_max + Math.abs(orig_min))) * 100);
        var perc = get_perc(i, min_perc, max_perc, zero_loc);
        var color = getColorForPercentage(perc);

        if (i == zero_loc)                           {
            color = "black";
        }
        if (orientation == "vertical" && value == i) {
            color = "white";
        }

        if (orientation == "vertical")
        {
            html += '<tr><td style="background-color:' + color + '; ' + dimension + '"></td></tr>';
        }
        else
        {
            html +=     '<td style="background-color:' + color + '; ' + dimension + '"></td>';
        }
    }
      
    if (orientation == "horizontal") {
        html += '</tr>';
    }
  
    html += '</table>';
    
    return html;
}

function bind_inputs(default_min, default_max)
{
    $("#left_slider_value").bind('keyup change click', function (e) {
        if (! $(this).data("previousValue") || 
            $(this).data("previousValue") != $(this).val()
            )
            {
            var right_side = Math.min(Math.abs($(this).val()), default_max);
            slider_range.slider("values", [$(this).val(), right_side]);
            $("#right_slider_value").val(right_side);
            $("#gradient_cell").html(draw_gradient("horizontal", default_min, default_max, $(this).val(), right_side, 0));

        }
          
    });
  
  
    $("#left_slider_value").each(function () {
        $(this).data("previousValue", $(this).val());
    });

    $("#right_slider_value").bind('keyup change click', function (e) {
        if (! $(this).data("previousValue") || 
            $(this).data("previousValue") != $(this).val()
            )
            {
            var left_side = -1 * Math.max($(this).val(), default_min);
            slider_range.slider("values", [left_side, $(this).val()]);
            $("#left_slider_value").val(left_side);
            $("#gradient_cell").html(draw_gradient("horizontal", default_min, default_max, left_side, $(this).val(), 0));

        }
        
    });


    $("#right_slider_value").each(function () {
        $(this).data("previousValue", $(this).val());
    });
}


function adjust_column(column)
{
    var props = getProperties();
    var min = props[column]['min'];
    var max = props[column]['max'];
    var default_min = defaultColumns[column]['min'];
    var default_max = defaultColumns[column]['max'];
    var step = (default_max - default_min) / 100;
  
    var color_scale = document.getElementById('color_scale');
    color_scale.style.display = "block";
    color_scale.innerHTML = '<table style="width:100%"><tr><th>' + column + '</th></tr><tr><td id="gradient_cell"></td></tr><tr><td><table style="width:100%"><tr><td><input class="slider_value" type="number" min="' + default_min + '" max="0" step="' + step + '" id="left_slider_value"></input></td><td style="width:100%"><div style="width:85%; margin-left:auto;margin-right:auto" id="slider"></td><td><input class="slider_value"  type="number" min="0" max="' + default_max + '" step="' + step + '"  id="right_slider_value"></input></td></tr></div></table></td></tr><tr><th><button class="scale_button"  onclick="apply_range(' + "'" + column + "'" + ')">Apply</button><button class="scale_button" onclick="cancel_range(' + "'" + column + "'" + ')">Cancel</button></th></tr></table>';
    
    bind_inputs(default_min, default_max);
    
    slider_range = $("#slider");

    var updateSliderValue = function (e, ui) {
        var which_handle = ui.handle.id;
    
        if (which_handle == "left")
        {
            var right_val = Math.min(Math.abs(ui.values[0]), default_max);
            slider_range.slider("values", [ui.values[0], right_val]);
            var tmp = right_val;
            $("#right_slider_value").val(tmp.toFixed(1));
            tmp = ui.values[0];
            $("#left_slider_value").val(tmp.toFixed(1));
        }
        else
        {
            var left_val = Math.max(-1 * ui.values[1], default_min);
            slider_range.slider("values", [left_val, ui.values[1]]);
            var tmp = left_val;
            $("#left_slider_value").val(tmp.toFixed(1));
            tmp = ui.values[1];
            $("#right_slider_value").val(tmp.toFixed(1));
        }
    
        $("#gradient_cell").html(draw_gradient("horizontal", default_min, default_max, ui.values[0], ui.values[1], 0));
    };

    slider_range.slider({
        min: default_min,
        max: default_max,
        step: step,
        range: true,
        slide: updateSliderValue,
        values: [min, max]
    });
  

    var as = document.getElementById('slider').getElementsByTagName("a");

    var left_found = false;
    for (var i in as) 
    {
        var thing = as[i];
        if (! thing || !thing.style) {
            continue;
        }
        var left = thing.style.left;

        if (!left_found) {
            thing.id = "left";
            left_found = true;
        } else {
            thing.id = "right";
        }
    }

    //$("#left").css("width", "5px");
    //$("#right").css("width", "5px");
    $("#left_slider_value").val(min);
    $("#right_slider_value").val(max);
    $("#gradient_cell").html(draw_gradient("horizontal", default_min, default_max, min, max, 0));
  
  
    var coordinates = getOffset(document.getElementById(column));
    $("#color_scale").css("top", coordinates.top);
    $("#color_scale").css("left", coordinates.left);
  
    columns[column] = 1;
    color_slices_by_metadata(Object.keys(columns));
    document.getElementById(column).checked = true;
}

function getOffset( el ) {
    var _x = 0;
    var _y = 0;
    while( el && !isNaN( el.offsetLeft ) && !isNaN( el.offsetTop ) ) {
        _x += el.offsetLeft ;//- el.scrollLeft;
        _y += el.offsetTop ;//- el.scrollTop;
        el = el.offsetParent;
    }
    return {
        top: _y, 
        left: _x
    };
}

function fill_dynamic_metadata_table()
{
    var html_str   = '';
    var metadata   = getMetadata();
    var properties = getProperties();
  
    var col_array = Object.keys(properties);
    var row_array = new Array();
  
    for (var i in json.nodes)
    {
        var node = json.nodes[i];
        if (node_has_metadata(node.name))
        {
            row_array.push(node.name);
        }
    } 
  
    html_str += "<tr><th>Node Name</th>\n";
    for (var c in col_array)
    {
        html_str += "<th>" + col_array[c] + "</th>\n";
    }
    html_str += "</tr>\n";
  
    for (var r in row_array)
    {
        var row = row_array[r];
    
        html_str += "<tr>\n";
        html_str += "<td>" + row + "</td>";
        for (var c in col_array)
        {
            var col   = col_array[c];
            html_str += '<td style="text-align:right">' + format_number(metadata[col][row]) + "</td>\n";
        }
        html_str += "</tr>\n";
    }
  
    document.getElementById("dynamic-metadata").innerHTML = html_str;
}

function link_dist_down()
{
    var value = Number(document.getElementById('linkdisttextbox').value);
    document.getElementById('linkdisttextbox').value = value - 10;
    update_force();
}

function link_dist_up()
{
    var value = Number(document.getElementById('linkdisttextbox').value);
    document.getElementById('linkdisttextbox').value = value + 10;
    update_force();
}

function charge_down()
{
    var value = Number(document.getElementById('chargetextbox').value);
    document.getElementById('chargetextbox').value = value - 100;
    update_force();
}

function charge_up()
{
    var value = Number(document.getElementById('chargetextbox').value);
    document.getElementById('chargetextbox').value = value + 100;
    update_force();
}

function is_subgraph_page()
{
    var title = document.getElementById('mainTitle').textContent.toLowerCase();
  
    if (title.indexOf('clique') >= 0)
    {
        return false;
    }
    return true;
}

function fill_dynamic_form()
{
    var html_str = '<table cellpadding="0" cellspacing="5" border="0" valign="top" style="background-color:inherit;white-space:nowrap;"><tr style="background-color:inherit;">' + 
    '<td style="background-color:inherit;">'; 
                 
    if (is_subgraph_page())
    {
        html_str += '<table cellpadding="0" cellspacing="5" border="0" valign="top" style="    background-color:inherit;white-space:nowrap;"><tr><td>Charge: </td><td><table cellpadding="0" cellspacing="0" border="0"><tr><td rowspan="2"><input id="chargetextbox" type="text" name="number" value="-2000" style="width:50px;height:23px;font-weight:bold;" onkeydown="update_force()" /></td><td><input type="button" value=" + "  onclick="charge_up();" style="font-size:7px;margin:0;padding:0;width:20px;height:13px;" ></td></tr><tr><td><input type=button value=" - " onclick="charge_down();" style="font-size:7px;margin:0;padding:0;width:20px;height:12px;" ></td></tr></table></td></tr>' +
    '<tr><td>Link Dist: </td><td><table cellpadding="0" cellspacing="0" border="0"><tr><td rowspan="2"><input id="linkdisttextbox" type="text" name="number" value="150" style="width:50px;height:23px;font-weight:bold;" onkeydown="update_force()" /></td><td><input type="button" value=" + "  onclick="link_dist_up();" style="font-size:7px;margin:0;padding:0;width:20px;height:13px;" ></td></tr><tr><td><input type=button value=" - " onclick="link_dist_down();" style="font-size:7px;margin:0;padding:0;width:20px;height:12px;" ></td></tr></table></td></tr></table>';
    }
  
    html_str += '</td><td><table cellpadding="0" cellspacing="5" border="0" valign="top" style="    background-color:inherit;white-space:nowrap;"><tr><td>' +
    '<input type="radio" name="colorby" value="Color by cliques" onclick="remove_checkboxes(); color_by_clique();" checked>Color by cliques</input>' + 
    '<input type="radio" name="colorby" value="Color by species" onclick="remove_checkboxes(); color_by_species();">Color by species</input>' + 
    '<input type="radio" name="colorby" value="Color by metadata" onclick="create_checkboxes();">Color by metadata</input>' + 
    '<input type="radio" name="colorby" value="lucky" onclick="create_checkboxes(); randomize_checkboxes();">Im Feelin Lucky</button>' + 
    '</td></tr>' +
    '<tr><td colspan="2"><input id="nodetextvisiblecheckbox" type="checkbox" name="togglenodetextvisible" value="togglenodetextvisible" onclick="toggle_node_text_visible();" >Show All Node Names</input></td></tr>' +
    '<tr><td><span id="checkboxspan"></span></td></tr></table>';
                  
    document.getElementById("dynamic-form").innerHTML = html_str;
}


function create_checkboxes()
{
    columns = new Object();
    var html_str = '<input id="textvisiblecheckbox" type="checkbox" name="toggletextvisible" value="toggletextvisible" onclick="toggle_metadata_text_visible();" checked>Show Values</input></br>';
    var properties = getProperties();
    var col_array = Object.keys(properties);
  
    for (var i in col_array)
    {
        var col = col_array[i];
        var color_style = 'background-color:' + get_slice_base_color(col) + ';';
        if (properties[col]['type'].toLowerCase() == 'r')
        {
            color_style = 'background-size: cover;background-repeat: no-repeat;background-image:url(' + get_base_dir() + 'include/redgreen.png);';
            html_str += '<span class="metadatacheckboxspan" style="' + color_style + '"><input  class="metadatacheckbox" type="checkbox" name="colorbymetadata" id="' + col + '" value="' + col + "\" onclick=\"toggle_column('" + col + "');\"></input><a  onclick=\"adjust_column('" + col + "')\" style=\"text-decoration:underline\">" + col + '</a></span>';
        }
        else
        {
            html_str += '<span class="metadatacheckboxspan" style="' + color_style + '"><input  class="metadatacheckbox" type="checkbox" name="colorbymetadata" id="' + col + '" value="' + col + "\" onclick=\"toggle_column('" + col + "');\"></input><a>" + col + '</a></span>';
        }
        if (i % 5 == 4) html_str += '</br>';
    }
  
    document.getElementById("checkboxspan").innerHTML = html_str;
}

function remove_checkboxes()
{
    document.getElementById("checkboxspan").innerHTML = "";
}

function randomize_checkboxes()
{
    var checkboxes = document.getElementsByClassName("metadatacheckboxspan");
    columns = new Object();
    for (var i in checkboxes)
    {
        var checkboxspan = checkboxes[i];
    
        if (Math.random() > 0.5)
        {
            checkboxspan.firstChild.setAttribute("checked", "");
            toggle_column(checkboxspan.textContent);
        }
        else
        {
            checkboxspan.firstChild.removeAttribute("checked");
        }
    }
}

function UpdateTableHeaders()
{
    $(".persist-area").each(function()
    {
        var el         = $(this),
        offset         = el.offset(),
        scrollTop      = $(window).scrollTop(),
        floatingHeader = $(".floatingHeader", this)
         
        if ((scrollTop > offset.top) && (scrollTop < offset.top + el.height()))
        {
            floatingHeader.css({
                "visibility" : "visible"
            });
        }
        else
        {
            floatingHeader.css({
                "visibility" : "hidden"
            });      
        };
    });
}
  

// DOM Ready      
$(function()
{
    var clonedHeaderRow;
  
    $(".persist-area").each(function()
    {
        clonedHeaderRow = $(".persist-header", this);
        clonedHeaderRow
        .before(clonedHeaderRow.clone())
        .css("width", clonedHeaderRow.width())
        .addClass("floatingHeader");
    });
     
    $(window)
    .scroll(UpdateTableHeaders)
    .trigger("scroll");
});


function toggle2(showText, showHideDiv, switchTextDiv)
{
    var ele  = document.getElementById(showHideDiv);
    var text = document.getElementById(switchTextDiv);
    if (ele.style.display == "block")
    {
        ele.style.display = "none";
        text.innerHTML    = showText + " v";
    }
    else
    {
        ele.style.display = "block";
        text.innerHTML    = showText + " ^";
    }
}

  
function toggle3(showText, contentDiv, controlDiv)
{
    if (contentDiv.constructor == Array)
    {
        for (i = 0; i < contentDiv.length; i++)
        {
            toggle2('dork', contentDiv[i], controlDiv[i]);
        }
    }
    else
    {
        toggle2('junk', contentDiv, controlDiv);
    }
}


function get_radius(num_nodes, min_dist, max_radius)
{
    var angle  = (360 / num_nodes) * 0.01745239;
    var radius = max_radius + 100;
    var buf    = 55;

    while (radius > max_radius - buf)
    {
        radius = Math.sqrt(((Math.pow(min_dist, 2)) / (2*(1 - (Math.cos(angle))))));
        min_dist--;
    }

    return radius;
}


function get_points(num_nodes, min_dist, x_origin, y_origin)
{
    var new_angle   = 0;
    var angle_delta = (360 / num_nodes)
    var radius      = get_radius(num_nodes, min_dist, Math.min(x_origin, y_origin));
    var points      = new Array();

    while (new_angle < 360)
    {
        var rangle = new_angle * 0.01745239;
        var x      = Math.floor(radius * Math.cos(rangle)) + x_origin;
        var y      = Math.floor(radius * Math.sin(rangle)) + y_origin;
        var point  = new Object();
        point.x = x;
        point.y = y;
        points.push(point);

        new_angle += angle_delta;
    }
    
    return points;
}


