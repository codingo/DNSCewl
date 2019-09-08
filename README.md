# DNSCewl
A DNS Bruteforcing Wordlist Generator

Goal: when provided with a list of domain names, generate a wordlist of potential subdomains to be tested for.

Take a list of domain names (using -tL) or a single target (using -t), a single request will be performed against each of them and then the top 100 words that are not HTML related will be added to our wordlis.

| Argument   | Description                                                           |
|------------|-----------------------------------------------------------------------|
| -t         | Specify a single target.                                              |
| -tL        | Specify a list of targets.                                            |
| -sL        | Specify a list of sets to substitue with                              |
| -eL        | Specify a list of targets to exclude                                  |
| -eX        | Specify a list of domain extensions to substitute with |
| -a         | Specify a file of words to append to a host                           |
| -p         | Specify a file of words to prepend to a host                          |
| -v         | If set then verbose output will be displayed in the terminal          |
| -i         | If set, original domains (from source files) are included in the output. |
| --range    | Set a higher range for integer permutations. |
| --subs     | If set then only subdomains will be generated. |
| --no-color | If set then any foreground or background colours will be stripped out |
| --limit     | Specify a fixed word limit to output.                                 |
| --level     | Specify the level of results to output. |
| --no-repeats | Prevent repeated structures such as one.one.com |

# Level
There are multiple levels of changes that can be performed, specified by "-level". A level of "1" or above modifies the results to use a `-` as well as everything else. A level of 0 wouldn't use `-`. Level defaults to level 1.

At level 2 integer substituions start to take place, using a set range.




# Append
Append works by adding a new subdomain, as well as the word with a `-` and without (unless level is 0). So, for example:

A target list of:
```
example.com
help.example.com
```

With an append list of:
```
dev
test
```

With command line as:

```
DNSCewl -tL targets.txt -a append.txt
```

Would provide the output:

```
example-dev.com
example-test.com
help-dev.example.com
help-test.example.com
example.dev.com
example.test.com
help.dev.example.com
help.test.example.com
```

If the flag `--subs` was passed then this would limit results to new subdomains only, and output the following:

```
dev.example.com
devhelp.example.com
dev-help.example.com
dev.help.example.com
test.example.com
testhelp.example.com
test-help.example.com
test.help.example.com
```

# Prepend

Prepend is the same as append, but at the beginning of a domain (or subdomain). 

Using as:

```
DNSCewl -tL targets.txt -p append.txt
```

Would result in:

```
devexample.com
dev-example.com
dev.example.com
devhelp.example.com
dev-help.example.com
dev.help.example.com
testexample.com
test-example.com
test.example.com
testhelp.example.com
test-help.example.com
test.help.example.com
```

# Level Usage Example
If level 1 was spefied, results with `-` wouldn't be used. For example:

```
DNSCewl -tL targets.txt -p append.txt --level=0
```

Would result in:

```
devexample.com
dev.example.com
devhelp.example.com
dev.help.example.com
testexample.com
test.example.com
testhelp.example.com
test-help.example.com
test.help.example.com
```

# Subs Usage Example
Subs limits results to subdomains only. For example:


```
DNSCewl -tL targets.txt -p append.txt --level=0 --subs
```

Would result in:

```
dev.example.com
dev.help.example.com
test.example.com
test-help.example.com
test.help.example.com
```

# No repeats example
No repeats prevents the same term being used twice in a domain.

For example, using `test.example.com` with an prepended list of:

```
test
```

With the following:

```
DNSCewl -tL targets.txt -p append.txt --no-repeats
```

Would result in:

```
test.example.com
test.testexample.com
```

Note that test.test.example.com isn't included, as it's a repeated subdomain. It's important to note that test.testexample.com is still possible here, as --no-repeats shouldn't prevent a subdomain and a top level domain from repeating.

# Set List (-sL) Example
A set list is a series of words to perform a replacement on when one word in a set is discovered.

For example, an input of:

```
one.example.com
b.example.com
```

When provided a set list of:

```
one
two
three
```

Would output:

```
two.example.com
three.example.com
```

# Include originals (-i)
If set than the original domains, from source lists are included in outputted results. If not then they will be excluded.

# Range Example
By default if level=2 is set then any integers in a domain would be incremented and decremented by 100. For example,

```
101.example.com
```

Would generate domains from 1.example.com to 200.example.com as output.

Range allows you to override this setting. By default range will adjust to be both negative and positive. For example:

```
--range=10
```
Would generate domians from 91.example.com to 111.example.com. You can however specify + or - on range. So for example,

```
--range=+10
```

Would only generate domains from 101.example.com to 111.example.com as our output.

# Domain Extensions (-eX)
Takes a list of domain extensions and replaces extensions on final results with them.

For example,

```
example.com
sub.example.com
```

When combined with a list of extentions that includes:

```
.com.au
.co.uk
```

Would result in a list of domains:

```
example.com.au
example.co.uk
sub.example.com.au
sub.example.co.uk
```
