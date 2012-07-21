class QAST::Block is QAST::Node {
    has str $!blocktype;
    has int $!lexical;
    has int $!custom_args;
    has str $!cuid;
    has %!symbol;
    
    method blocktype(*@value)   { $!blocktype := @value[0] if @value; $!blocktype }
    method lexical(*@value)     { $!lexical := @value[0] if @value; $!lexical }
    method custom_args(*@value) { $!custom_args := @value[0] if @value; $!custom_args }
    
    my $cur_cuid := 0;
    my $cuid_suffix := ~nqp::time_n();
    
    method cuid(*@value) {
        if @value {
            # Set ID if we are provided one.
            $!cuid := @value[0];
        }
        elsif $!cuid {
            # If we already have an ID, return it.
            $!cuid
        }
        else {
            # Otherwise, generate one.
            $cur_cuid := $cur_cuid + 1;
            $!cuid := 'cuid_' ~ $cur_cuid ~ '_' ~ $cuid_suffix;
        }
    }

    method symbol($name, *%attrs) {
        if %attrs {
            %!symbol{$name} := %!symbol{$name} // {};
            for %attrs {
                %!symbol{$name}{$_.key} := $_.value;
            }
        }
        %!symbol{$name}
    }
    
    method symtable() {
        %!symbol
    }
}