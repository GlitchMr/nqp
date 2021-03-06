#! nqp

# Tests for try and catch

plan(7);

sub oops($msg = "oops!") { # throw an exception
    nqp::die($msg);
}

my $ok := 1;
try {
    oops();
    $ok := 0;
}
ok($ok, "exceptions exit a try block");

sub foo() {
    try {
        return 1;
    }
    return 0;
}

ok(foo(), "control exceptions are not caught by a try block");

ok(nqp::istype((try oops()), NQPMu), "statement prefix form of try works");

{
    CATCH { ok(1, "CATCH blocks are invoked when an exception occurs"); }
    oops();
}


$ok := 1;
sub bar() {
    CATCH { $ok := 0; }
    return;
}
bar();
ok($ok, "CATCH blocks ignore control exceptions");

$ok := 1;
{
    {
        {
            {
                oops();
                CATCH { $ok := $ok * 2; nqp::rethrow($!); }
            }
            CATCH { $ok := $ok * 2; nqp::rethrow($!); }
        }
        CATCH { $ok := $ok * 2; nqp::rethrow($!); }
    }
    CATCH { ok($ok == 8, "rethrow and multiple exception handlers work") }
}

$ok := 1;

{
    for 1, 2, 3, 4 {
        $ok := $ok * 2;
        oops();
    }
    CATCH { my &c := $!<resume>; &c(); }
}

ok($ok == 16, "resuming from resumable exceptions works");
