#!/usr/bin/perl

#$rfile="test.vl";
#$wfile="test.c";

sub judge_str_type;
sub resolve_hex_bin_type;
sub resolve_bracket_type;

#$ARGV[0>; # first param   @ARGV; # number of param
$rfile=$ARGV[0];
$wfile=$ARGV[1];

if(@ARGV != 2){
    print "wrong parameter!!!, please use as follow:\n\n";
    print "perl convert.pl inputfile outfile\n\n";
    exit(0);
}

if(-e $rfile){
#    print $rfile," can be opened\n";
}else{
    print $rfile, " doesn't exist\n";
}

open(RFILE, $rfile)||die "open $rfile error\n";
open(WFILE, ">$wfile")||die "can't open $wfile file\n";

$firstpart = "#ifndef regdef_sl1_INCLUDED\n#define regdef_sl1_INCLUDED\n\n";
$lastpart =  "#endif // regdef_sl1_INCLUDED\n\n";

#First part in outfile
print WFILE $firstpart;

while(<RFILE>){
    $pos0=index($_,"`define");
    if($pos0 == 0){
    	# Handle comments inline
    	$pos1 = index($_, "//");
    	if ($pos1 != -1) {
    		($content, $comment)=split ("//", $_);
			$_= $content;
    	}
        print WFILE "#define";
		s/[ \t]+$//;
        $str=substr($_,7);
        
		$type = judge_str_type($str);
		if($type != -1){
		    $ret = resolve_hex_bin_type($type, $str);
		    print WFILE $ret;
		}else{
		    $ret = resolve_bracket_type($str);
		    print WFILE $ret;
		}
    }else{
        #comment line
        print WFILE $_;
    }
}#end while

#Last part in outfile
print WFILE $lastpart;
 
print "Convert completely!\n";


sub judge_str_type{
    my($str) = @_;
    
    $ret = index($str,"'b1");
    if($ret == -1){
        $ret = index($str,"'b0");
        if($ret == -1){
             $ret = index($str,"'h");
             if($ret != -1){$ret = 10000;}
        }
     } 
    return $ret;
}

sub resolve_hex_bin_type{
    my($type,$str)=@_;

    $ret = "";
    if($type == 10000){
        $str =~ s/[0-9]+\'h/0x/;
        $ret = $str;
    }else{
        $tmp =substr($str,$type+1);
        $tmp =~ s/b/0b/;
        $tmp = oct($tmp);
        $ret = substr($str,0,$type-1);
        $ret = sprintf("%s 0x%x\n",$ret,$tmp);
    }
    return $ret;
}

sub resolve_bracket_type{
    my($str) = @_;
    $ret = "";
    
    $pattern0="[\+*/0-9a-zA-Z_-]+";
    $pattern1="[\\t ]+";

    $str=~ s/\`//g;
    chomp($str);

    @result0=split(/$pattern0/,$str);
    @result1=split(/$pattern1/,$str);

    $i = 0;
    $len = @result1;
	while($i<$len){
        $t0 = @result1[$i];
        $t1 = @result0[$i];
        $ret = sprintf("%s%s%s", $ret, $t0,$t1);
	    if($i == 1){
             $ret = sprintf("%s(", $ret);
        }
	    $i += 1;
	}

    $ret = sprintf("%s)\n", $ret);
}
