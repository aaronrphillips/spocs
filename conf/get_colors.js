//REALLY IMPORTANT NOTE: <script src="../../include/color-0.4.1.js"></script>

  function getrgb(angle){
    angle = angle % 360;
    var r = 0;
    var g = 0;
    var b = 0;
    var adjust = 255;

    if (angle >= 300 || angle <= 60)
    {
      r = 255;
    }
    else if (angle > 60 && angle <= 120)
    {
      r = ((120 - angle) / 60) * adjust;
    }
    else if (angle > 240)
	{
      r = 255 - ((300 - angle) / 60) * adjust;
    }

    if (angle >= 60 && angle <= 180)
    {
      g = 255;
    }
    else if (angle < 60 && angle > 0)
    {
      g = 255 - ((60 - angle) / 60) * adjust;
    }
    else if (angle > 180 && angle < 240)
	{
      g = ((240 - angle) / 60) * adjust;
    }

    if (angle >= 180 && angle <= 300)
    {
      b = 255;
    }
    else if (angle < 180 && angle > 120)
    {
      b = 255 - ((180 - angle) / 60) * adjust;
    }
    else if (angle > 300)
	{
      b = ((360 - angle) / 60) * adjust;
    }


    r = Math.floor(Math.min(r, 255));
	g = Math.floor(Math.min(g, 255));
    b = Math.floor(Math.min(b, 255));

    var gb = (g << 8) | (r << 16);
    r = r.toString(16);
    g = g.toString(16);
    b = b.toString(16);
    if (r.length == 1) r = '0' + r;
    if (g.length == 1) g = '0' + g;
    if (b.length == 1) b = '0' + b;

    return '#' + r + g + b;
  }

  function get_colors(num_colors)
  {
    var angle_delta = Math.floor(360 / (num_colors));
    document.write("<table><tr>");
    var angle = 0;
    for (var count = 0; count < num_colors; count++, angle += angle_delta)
    {
      var color = Color(getrgb(angle));
      if (count % 3 == 2) color.lighten(.6);
      if (count % 3 == 1) color.lighten(.3);
      document.write("<td bgcolor=\"" + color.hexString() + "\">" + ":::"  + "</td>");
    }
    document.write("</tr></table>");
  }

  //get_colors(33);
