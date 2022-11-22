NAMESPACE=netier
for x in $(find src -name "*.cpp"); do
    sed -i "$(grep -n "#include" $x | tail -1 | sed 's/:.*//')a\\\nnamespace $NAMESPACE {\n" $x
    echo >> $x
    echo "} // namespace $NAMESPACE" >> $x
done

for x in $(find include -name "*.hpp"); do
    sed -i "$(grep -n "#include" $x | tail -1 | sed 's/:.*//')a\\\nnamespace $NAMESPACE {\n" $x
    echo >> $x
    echo "} // namespace $NAMESPACE" >> $x
done