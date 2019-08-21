# DNSCewl
A DNS Bruteforcing Wordlist Generator

```
Some inspiration here: https://github.com/elceef/dnstwist

- Evaluates web page similarity with fuzzy hashes to find live phishing sites

```

Goal: when provided with a list of domain names, generate a wordlist of potential subdomains to be tested for.

Take a list of domain names (using -tL) or a single target (using -t), a single request will be performed against each of them and then the top 100 words that are not HTML related will be added to our wordlis.

| Argument   | Description                                                           |
|------------|-----------------------------------------------------------------------|
| -t         | Specify a single target.                                              |
| -tL        | Specify a list of targets.                                            |
| -eL        | Specify a list of targets to exclude                                  |
| -a         | Specify a file of words to append to a host                           |
| -p         | Specify a file of words to prepend to a host                          |
| -v         | If set then verbose output will be displayed in the terminal          |
| --subs     | If set then only subdomains will be generated. |
| --no-color | If set then any foreground or background colours will be stripped out |
| -limit     | Specify a fixed word limit to output.                                 |
| -level     | Specify the level of results to output. |
| --no-repeats | Prevent repeated structures such as one.one.com |

Level works in the following manner:

1 - Permute with and without a -

Append works by adding a new subdomain, as well as the word with a `-` and without. So, for example:

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
