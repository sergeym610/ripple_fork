/* -------------------------------- REQUIRES -------------------------------- */

var child = require("child_process");
var assert = require("assert");

/* --------------------------------- CONFIG --------------------------------- */

if (process.argv[2] == null) {
  [
   'Usage: ',
   '',
   '  `node bin/stop-test.js i,j [cbcd_path] [cbcd_conf]`',
   '',
   '  Launch cbcd and stop it after n seconds for all n in [i, j}',
   '  For all even values of n launch cbcd with `--fg`',
   '  For values of n where n % 3 == 0 launch cbcd with `--fg`\n',
   'Examples: ',
   '',
   '  $ node bin/stop-test.js 5,10',
   ('  $ node bin/stop-test.js 1,4 ' +
      'build/clang.debug/cbcd $HOME/.confs/cbcd.cfg')
   ]
      .forEach(function(l){console.log(l)});

  process.exit();
} else {
  var testRange = process.argv[2].split(',').map(Number);
  var cbcdPath = process.argv[3] || 'build/cbcd'
  var cbcdConf = process.argv[4] || 'cbcd.cfg'
}

var options = {
  env: process.env,
  stdio: 'ignore' // we could dump the child io when it fails abnormally
};

// default args
var conf_args = ['--conf='+cbcdConf];
var start_args  = conf_args.concat([/*'--net'*/])
var stop_args = conf_args.concat(['stop']);

/* --------------------------------- HELPERS -------------------------------- */

function start(args) {
    return child.spawn(cbcdPath, args, options);
}
function stop(cbcd) { child.execFile(cbcdPath, stop_args, options)}
function secs_l8r(ms, f) {setTimeout(f, ms * 1000); }

function show_results_and_exit(results) {
  console.log(JSON.stringify(results, undefined, 2));
  process.exit();
}

var timeTakes = function (range) {
  function sumRange(n) {return (n+1) * n /2}
  var ret = sumRange(range[1]);
  if (range[0] > 1) {
    ret = ret - sumRange(range[0] - 1)
  }
  var stopping = (range[1] - range[0]) * 0.5;
  return ret + stopping;
}

/* ---------------------------------- TEST ---------------------------------- */

console.log("Test will take ~%s seconds", timeTakes(testRange));

(function oneTest(n /* seconds */, results) {
  if (n >= testRange[1]) {
    // show_results_and_exit(results);
    console.log(JSON.stringify(results, undefined, 2));
    oneTest(testRange[0], []);
    return;
  }

  var args = start_args;
  if (n % 2 == 0) {args = args.concat(['--fg'])}
  if (n % 3 == 0) {args = args.concat(['--net'])}

  var result = {args: args, alive_for: n};
  results.push(result);

  console.log("\nLaunching `%s` with `%s` for %d seconds",
                cbcdPath, JSON.stringify(args), n);

  cbcd = start(args);
  console.log("cbcd pid: %d", cbcd.pid);

  // defaults
  var b4StopSent = false;
  var stopSent = false;
  var stop_took = null;

  cbcd.once('exit', function(){
    if (!stopSent && !b4StopSent) {
      console.warn('\ncbcd exited itself b4 stop issued');
      process.exit();
    };

    // The io handles close AFTER exit, may have implications for
    // `stdio:'inherit'` option to `child.spawn`.
    cbcd.once('close', function() {
      result.stop_took = (+new Date() - stop_took) / 1000; // seconds
      console.log("Stopping after %d seconds took %s seconds",
                   n, result.stop_took);
      oneTest(n+1, results);
    });
  });

  secs_l8r(n, function(){
    console.log("Stopping cbcd after %d seconds", n);

    // possible race here ?
    // seems highly unlikely, but I was having issues at one point
    b4StopSent=true;
    stop_took = (+new Date());
    // when does `exit` actually get sent?
    stop();
    stopSent=true;

    // Sometimes we want to attach with a debugger.
    if (process.env.ABORT_TESTS_ON_STALL != null) {
      // We wait 30 seconds, and if it hasn't stopped, we abort the process
      secs_l8r(30, function() {
        if (result.stop_took == null) {
          console.log("cbcd has stalled");
          process.exit();
        };
      });
    }
  })
}(testRange[0], []));