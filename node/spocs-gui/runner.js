'use strict'

var exec = require('child_process').execFile;
var path = require('path');

/*
{ 
  fasta_files: [ 'uploads/a.fasta', 'uploads/b.fasta', 'uploads/c.fasta' ],
  outgroup_file: 'uploads/c.fasta',
  output_html: true 
}
*/
var spocs_root = path.join(__dirname, "public", "spocs");
var paralogs_dir = path.join(spocs_root, "paralogs");
var report_dir = path.join(spocs_root, "report");

function run(params, callback) {
  console.log("running", params);
  var args = ['-s'];
  args = args.concat(params.fasta_files);
  args = args.concat(['-p', paralogs_dir, '-R', report_dir]);
  if (params.outgroup_file) {
    args.push('-o');
    args.push(params.outgroup_file);
  }
  if (params.output_html) {
    args.push('-H');
  }

  console.log("spocs " + args.join(" "));

  exec('spocs', args, {}, (err, stdout, stderr) => {
    if (err) {
      console.error("ERROR: " + err);
      throw err;
    }
    console.log("Spocs has finished");
    callback();
  });

  console.log("done with run (waiting for exec)");
}

module.exports = { run };
