CC=gcc
CARGS=-Wall -O3
GMPLIB=-L/gmp_install/lib -lgmp
CMPHLIB=-L/usr/local/lib/libcmph.la -lcmph

all:
	$(CC) $(CARGS) parameterised_matching.c -o parameterised_matching $(GMPLIB) $(CMPHLIB)

clean:
	rm parameterised_matching

karp-rabin:
	$(CC) $(CARGS) karp_rabin.c -o karp_rabin $(GMPLIB)

karp-rabin-clean:
	rm karp_rabin

hash-lookup:
	$(CC) $(CARGS) hash_lookup.c -o hash_lookup $(CMPHLIB)

hash-lookup-clean:
	rm hash_lookup

mmatch:
	$(CC) m_match.c -o m_match $(CMPHLIB)

mmatch-clean:
	rm m_match
