#!/usr/bin/env ruby

OUTDIR = 'testout'
`mkdir #{OUTDIR}` unless File.directory? OUTDIR

DEF = /^(void|int)\s+sys_.+\(.*\)$/
Defines = Hash.new
File.readlines('fs.h').each do |l|
	l.strip!
	next unless l =~ DEF
	t = l.tr('(),', ' ').split(' ')
	Defines[t[1]] = t
end
fns = Defines.map {|k,v| k}
Dir.chdir OUTDIR

def gen_call(fn, prefix, ret)
	l = []
	d = Defines[fn]
	l << "#{d[0]} #{ret};" if d[0] != 'void'
	args = d[2..-1].each_slice(2).map{|e| "#{prefix}_#{e.last}"}
	t = ''
	t = "#{ret} = " if d[0] != 'void'
	l << "#{t} #{fn}(#{args.join ','});"
	l.join "\n"
end

def gen_one_body(fn, prefix)
	l = []
	d = Defines[fn]
	#l << "#{d[0]} #{prefix}_ret;" if d[0] != 'void'
	args = []
	d[2..-1].each_slice(2).each do |arg|
		vn = "#{prefix}_#{arg[1]}"
		case arg[0]
		when 'int'
			l << "#{arg[0]} #{vn};"
		else
			fail "error type #{arg[0]}"
		end
		args << vn
		l << "klee_make_symbolic(&#{vn}, sizeof(#{vn}), \"#{vn}\");"
	end
	#call
	t = ''
	t = "#{prefix}_ret = " if d[0] != 'void'
	t += "#{fn}(#{args.join ','});"
	#l << t

	l.join "\n"
end

fns.repeated_combination(2).each do |pairs|
	p pairs
	f = File.open "test_#{pairs[0]}___#{pairs[1]}.c", "w"
	f.puts '#include "../fs.h"'
	a = gen_one_body pairs[0], "a"
	b = gen_one_body pairs[1], "b"
	f.puts %Q{
int main(){
	__init_os();
	#{a}
	#{b}
	
	__push_state(0);
	#{gen_call(pairs[0], "a", "ab_a_ret")}
	#{gen_call(pairs[1], "b", "ab_b_ret")}
	__push_state(1);
	__pop_state(0);
	#{gen_call(pairs[1], "b", "ba_b_ret")}
	#{gen_call(pairs[0], "a", "ba_a_ret")}
	__push_state(2);
	return 0;
}
	}
	f.close
end


